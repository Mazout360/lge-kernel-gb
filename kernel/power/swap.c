/*
 * linux/kernel/power/swap.c
 *
 * This file provides functions for reading the suspend image from
 * and writing it to a swap partition.
 *
 * Copyright (C) 1998,2001-2005 Pavel Machek <pavel@suse.cz>
 * Copyright (C) 2006 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 *
 */

#include <linux/module.h>
#include <linux/file.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/genhd.h>
#include <linux/device.h>
#include <linux/buffer_head.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/pm.h>

#include "power.h"

#define SWSUSP_SIG	"S1SUSPEND"

struct swsusp_header {
	char reserved[PAGE_SIZE - 20 - sizeof(sector_t) - sizeof(int)];
	sector_t image;
	unsigned int flags;	/* Flags to pass to the "boot" kernel */
	char	orig_sig[10];
	char	sig[10];
} __attribute__((packed));

static struct swsusp_header *swsusp_header;

/*
 * General things
 */

static unsigned short root_swap = 0xffff;
static struct block_device *resume_bdev;

struct block_device *hib_resume_bdev;
/*
 * Saving part
 */

static int mark_swapfiles(sector_t start, unsigned int flags)
{
	int error;

	hib_bio_read_page(swsusp_resume_block, swsusp_header, NULL);
	if (!memcmp("SWAP-SPACE",swsusp_header->sig, 10) ||
	    !memcmp("SWAPSPACE2",swsusp_header->sig, 10)) {
		memcpy(swsusp_header->orig_sig,swsusp_header->sig, 10);
		memcpy(swsusp_header->sig,SWSUSP_SIG, 10);
		swsusp_header->image = start;
		swsusp_header->flags = flags;
		error = hib_bio_write_page(swsusp_resume_block,
					swsusp_header, NULL);
	} else {
		printk(KERN_ERR "PM: Swap header not found!\n");
		error = -ENODEV;
	}
	return error;
}

/**
 *	swsusp_swap_check - check if the resume device is a swap device
 *	and get its index (if so)
 */

static int swsusp_swap_check(void) /* This is called before saving image */
{
	int res;

	res = swap_type_of(swsusp_resume_device, swsusp_resume_block,
			&hib_resume_bdev);
	if (res < 0)
		return res;

	root_swap = res;
	res = blkdev_get(hib_resume_bdev, FMODE_WRITE);
	if (res)
		return res;

	res = set_blocksize(resume_bdev, PAGE_SIZE);
	if (res < 0)
		blkdev_put(hib_resume_bdev, FMODE_WRITE);

	return res;
}

/**
 *	write_page - Write one page to given swap location.
 *	@buf:		Address we're writing.
 *	@offset:	Offset of the swap page we're writing to.
 *	@bio_chain:	Link the next write BIO here
 */

static int write_page(void *buf, sector_t offset, struct bio **bio_chain)
{
	void *src;

	if (!offset)
		return -ENOSPC;

	if (bio_chain) {
		src = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
		if (src) {
			copy_page(src, buf);
		} else {
			WARN_ON_ONCE(1);
			bio_chain = NULL;	/* Go synchronous */
			src = buf;
		}
	} else {
		src = buf;
	}
	return hib_bio_write_page(offset, src, bio_chain);
}

/*
 *	The swap map is a data structure used for keeping track of each page
 *	written to a swap partition.  It consists of many swap_map_page
 *	structures that contain each an array of MAP_PAGE_SIZE swap entries.
 *	These structures are stored on the swap and linked together with the
 *	help of the .next_swap member.
 *
 *	The swap map is created during suspend.  The swap map pages are
 *	allocated and populated one at a time, so we only need one memory
 *	page to set up the entire structure.
 *
 *	During resume we also only need to use one swap_map_page structure
 *	at a time.
 */

#define MAP_PAGE_ENTRIES	(PAGE_SIZE / sizeof(sector_t) - 1)

struct swap_map_page {
	sector_t entries[MAP_PAGE_ENTRIES];
	sector_t next_swap;
};

/**
 *	The swap_map_handle structure is used for handling swap in
 *	a file-alike way
 */

struct swap_map_handle {
	struct swap_map_page *cur;
	sector_t cur_swap;
	unsigned int k;
};

static void release_swap_writer(struct swap_map_handle *handle)
{
	if (handle->cur)
		free_page((unsigned long)handle->cur);
	handle->cur = NULL;
}

static int get_swap_writer(struct swap_map_handle *handle)
{
	handle->cur = (struct swap_map_page *)get_zeroed_page(GFP_KERNEL);
	if (!handle->cur)
		return -ENOMEM;
	handle->cur_swap = alloc_swapdev_block(root_swap);
	if (!handle->cur_swap) {
		release_swap_writer(handle);
		return -ENOSPC;
	}
	handle->k = 0;
	return 0;
}

static int swap_write_page(struct swap_map_handle *handle, void *buf,
				struct bio **bio_chain)
{
	int error = 0;
	sector_t offset;

	if (!handle->cur)
		return -EINVAL;
	offset = alloc_swapdev_block(root_swap);
	error = write_page(buf, offset, bio_chain);
	if (error)
		return error;
	handle->cur->entries[handle->k++] = offset;
	if (handle->k >= MAP_PAGE_ENTRIES) {
		error = hib_wait_on_bio_chain(bio_chain);
		if (error)
			goto out;
		offset = alloc_swapdev_block(root_swap);
		if (!offset)
			return -ENOSPC;
		handle->cur->next_swap = offset;
		error = write_page(handle->cur, handle->cur_swap, NULL);
		if (error)
			goto out;
		clear_page(handle->cur);
		handle->cur_swap = offset;
		handle->k = 0;
	}
 out:
	return error;
}

static int flush_swap_writer(struct swap_map_handle *handle)
{
	if (handle->cur && handle->cur_swap)
		return write_page(handle->cur, handle->cur_swap, NULL);
	else
		return -EINVAL;
}

/**
 *	save_image - save the suspend image data
 */

static int save_image(struct swap_map_handle *handle,
                      struct snapshot_handle *snapshot,
                      unsigned int nr_to_write)
{
	unsigned int m;
	int ret;
	int nr_pages;
	int err2;
	struct bio *bio;
	struct timeval start;
	struct timeval stop;

	printk(KERN_INFO "PM: Saving image data pages (%u pages) ...     ",
		nr_to_write);
	m = nr_to_write / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	bio = NULL;
	do_gettimeofday(&start);
	while (1) {
		ret = snapshot_read_next(snapshot, PAGE_SIZE);
		if (ret <= 0)
			break;
		ret = swap_write_page(handle, data_of(*snapshot), &bio);
		if (ret)
			break;
		if (!(nr_pages % m))
			printk("\b\b\b\b%3d%%", nr_pages / m);
		nr_pages++;
	}
	err2 = hib_wait_on_bio_chain(&bio);
	do_gettimeofday(&stop);
	if (!ret)
		ret = err2;
	if (!ret)
		printk("\b\b\b\bdone\n");
	else
		printk("\n");
	swsusp_show_speed(&start, &stop, nr_to_write, "Wrote");
	return ret;
}

/**
 *	enough_swap - Make sure we have enough swap to save the image.
 *
 *	Returns TRUE or FALSE after checking the total amount of swap
 *	space avaiable from the resume partition.
 */

static int enough_swap(unsigned int nr_pages)
{
	unsigned int free_swap = count_swap_pages(root_swap, 1);

	pr_debug("PM: Free swap pages: %u\n", free_swap);
	return free_swap > nr_pages + PAGES_FOR_IO;
}

/**
 *	swsusp_write - Write entire image and metadata.
 *	@flags: flags to pass to the "boot" kernel in the image header
 *
 *	It is important _NOT_ to umount filesystems at this point. We want
 *	them synced (in case something goes wrong) but we DO not want to mark
 *	filesystem clean: it is not. (And it does not matter, if we resume
 *	correctly, we'll mark system clean, anyway.)
 */

int swsusp_write(unsigned int flags)
{
	struct swap_map_handle handle;
	struct snapshot_handle snapshot;
	struct swsusp_info *header;
	int error;

	error = swsusp_swap_check();
	if (error) {
		printk(KERN_ERR "PM: Cannot find swap device, try "
				"swapon -a.\n");
		return error;
	}
	memset(&snapshot, 0, sizeof(struct snapshot_handle));
	error = snapshot_read_next(&snapshot, PAGE_SIZE);
	if (error < PAGE_SIZE) {
		if (error >= 0)
			error = -EFAULT;

		goto out;
	}
	header = (struct swsusp_info *)data_of(snapshot);
	if (!enough_swap(header->pages)) {
		printk(KERN_ERR "PM: Not enough free swap\n");
		error = -ENOSPC;
		goto out;
	}
	error = get_swap_writer(&handle);
	if (!error) {
		sector_t start = handle.cur_swap;

		error = swap_write_page(&handle, header, NULL);
		if (!error)
			error = save_image(&handle, &snapshot,
					header->pages - 1);

		if (!error) {
			flush_swap_writer(&handle);
			printk(KERN_INFO "PM: S");
			error = mark_swapfiles(start, flags);
			printk("|\n");
		}
	}
	if (error)
		free_all_swap_pages(root_swap);

	release_swap_writer(&handle);
 out:
	swsusp_close(FMODE_WRITE);
	return error;
}

/**
 *	The following functions allow us to read data using a swap map
 *	in a file-alike way
 */

static void release_swap_reader(struct swap_map_handle *handle)
{
	if (handle->cur)
		free_page((unsigned long)handle->cur);
	handle->cur = NULL;
}

static int get_swap_reader(struct swap_map_handle *handle, sector_t start)
{
	int error;

	if (!start)
		return -EINVAL;

	handle->cur = (struct swap_map_page *)get_zeroed_page(__GFP_WAIT | __GFP_HIGH);
	if (!handle->cur)
		return -ENOMEM;

	error = hib_bio_read_page(start, handle->cur, NULL);
	if (error) {
		release_swap_reader(handle);
		return error;
	}
	handle->k = 0;
	return 0;
}

static int swap_read_page(struct swap_map_handle *handle, void *buf,
				struct bio **bio_chain)
{
	sector_t offset;
	int error;

	if (!handle->cur)
		return -EINVAL;
	offset = handle->cur->entries[handle->k];
	if (!offset)
		return -EFAULT;
	error = hib_bio_read_page(offset, buf, bio_chain);
	if (error)
		return error;
	if (++handle->k >= MAP_PAGE_ENTRIES) {
		error = hib_wait_on_bio_chain(bio_chain);
		handle->k = 0;
		offset = handle->cur->next_swap;
		if (!offset)
			release_swap_reader(handle);
		else if (!error)
			error = hib_bio_read_page(offset, handle->cur, NULL);
	}
	return error;
}

/**
 *	load_image - load the image using the swap map handle
 *	@handle and the snapshot handle @snapshot
 *	(assume there are @nr_pages pages to load)
 */

static int load_image(struct swap_map_handle *handle,
                      struct snapshot_handle *snapshot,
                      unsigned int nr_to_read)
{
	unsigned int m;
	int error = 0;
	struct timeval start;
	struct timeval stop;
	struct bio *bio;
	int err2;
	unsigned nr_pages;

	printk(KERN_INFO "PM: Loading image data pages (%u pages) ...     ",
		nr_to_read);
	m = nr_to_read / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	bio = NULL;
	do_gettimeofday(&start);
	for ( ; ; ) {
		error = snapshot_write_next(snapshot, PAGE_SIZE);
		if (error <= 0)
			break;
		error = swap_read_page(handle, data_of(*snapshot), &bio);
		if (error)
			break;
		if (snapshot->sync_read)
			error = hib_wait_on_bio_chain(&bio);
		if (error)
			break;
		if (!(nr_pages % m))
			printk("\b\b\b\b%3d%%", nr_pages / m);
		nr_pages++;
	}
	err2 = hib_wait_on_bio_chain(&bio);
	do_gettimeofday(&stop);
	if (!error)
		error = err2;
	if (!error) {
		printk("\b\b\b\bdone\n");
		snapshot_write_finalize(snapshot);
		if (!snapshot_image_loaded(snapshot))
			error = -ENODATA;
	} else
		printk("\n");
	swsusp_show_speed(&start, &stop, nr_to_read, "Read");
	return error;
}

/**
 *	swsusp_read - read the hibernation image.
 *	@flags_p: flags passed by the "frozen" kernel in the image header should
 *		  be written into this memeory location
 */

int swsusp_read(unsigned int *flags_p)
{
	int error;
	struct swap_map_handle handle;
	struct snapshot_handle snapshot;
	struct swsusp_info *header;

	*flags_p = swsusp_header->flags;
	if (IS_ERR(resume_bdev)) {
		pr_debug("PM: Image device not initialised\n");
		return PTR_ERR(resume_bdev);
	}

	memset(&snapshot, 0, sizeof(struct snapshot_handle));
	error = snapshot_write_next(&snapshot, PAGE_SIZE);
	if (error < PAGE_SIZE)
		return error < 0 ? error : -EFAULT;
	header = (struct swsusp_info *)data_of(snapshot);
	error = get_swap_reader(&handle, swsusp_header->image);
	if (!error)
		error = swap_read_page(&handle, header, NULL);
	if (!error)
		error = load_image(&handle, &snapshot, header->pages - 1);
	release_swap_reader(&handle);

	if (!error)
		pr_debug("PM: Image successfully loaded\n");
	else
		pr_debug("PM: Error %d resuming\n", error);
	return error;
}

/**
 *      swsusp_check - Check for swsusp signature in the resume device
 */

int swsusp_check(void)
{
	int error;

	hib_resume_bdev = open_by_devnum(swsusp_resume_device, FMODE_READ);
    if (!IS_ERR(hib_resume_bdev)) {
        set_blocksize(hib_resume_bdev, PAGE_SIZE);
		clear_page(swsusp_header);
		error = hib_bio_read_page(swsusp_resume_block,
					swsusp_header, NULL);
		if (error)
			goto put;

		if (!memcmp(SWSUSP_SIG, swsusp_header->sig, 10)) {
			memcpy(swsusp_header->sig, swsusp_header->orig_sig, 10);
			/* Reset swap signature now */
			error = hib_bio_write_page(swsusp_resume_block,
						swsusp_header, NULL);
		} else {
			error = -EINVAL;
		}

put:
		if (error)
			blkdev_put(hib_resume_bdev, FMODE_READ);
		else
			pr_debug("PM: Signature found, resuming\n");
	} else {
		error = PTR_ERR(hib_resume_bdev);
	}

	if (error)
		pr_debug("PM: Error %d checking image file\n", error);

	return error;
}

/**
 *	swsusp_close - close swap device.
 */

void swsusp_close(fmode_t mode)
{
	if (IS_ERR(hib_resume_bdev)) {
		pr_debug("PM: Image device not initialised\n");
		return;
	}

	blkdev_put(hib_resume_bdev, mode);
}

static int swsusp_header_init(void)
{
	swsusp_header = (struct swsusp_header*) __get_free_page(GFP_KERNEL);
	if (!swsusp_header)
		panic("Could not allocate memory for swsusp_header\n");
	return 0;
}

core_initcall(swsusp_header_init);
