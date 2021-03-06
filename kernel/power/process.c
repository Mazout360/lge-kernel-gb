/*
 * drivers/power/process.c - Functions for starting/stopping processes on 
 *                           suspend transitions.
 *
 * Originally from swsusp.
 */


#undef DEBUG

#include <linux/interrupt.h>
#include <linux/oom.h>
#include <linux/suspend.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/freezer.h>
#include <linux/delay.h>
#include <linux/wakelock.h>
#include <linux/workqueue.h>

/* 
 * Timeout for stopping processes
 */
#define TIMEOUT	(20 * HZ)

static int try_to_freeze_tasks(bool user_only)
{
	struct task_struct *g, *p;
	unsigned long end_time;
	unsigned int todo;
    bool wq_busy = false;
	struct timeval start, end;
	u64 elapsed_msecs64;
    unsigned int elapsed_msecs;
	unsigned int wakeup = 0;
    int sleep_usecs = USEC_PER_MSEC;

	do_gettimeofday(&start);

	end_time = jiffies + TIMEOUT;
    
    if (!user_only)
        freeze_workqueues_begin();
    
	while (true) {
		todo = 0;
		read_lock(&tasklist_lock);
		do_each_thread(g, p) {
			if (p == current || !freeze_task(p))
				continue;

			/*
			 * Now that we've done set_freeze_flag, don't
			 * perturb a task in TASK_STOPPED or TASK_TRACED.
			 * It is "frozen enough".  If the task does wake
			 * up, it will immediately call try_to_freeze.
			 */
			if (!task_is_stopped_or_traced(p) &&
			    !freezer_should_skip(p))
				todo++;
		} while_each_thread(g, p);
		read_unlock(&tasklist_lock);
		if (todo && has_wake_lock(WAKE_LOCK_SUSPEND)) {
			wakeup = 1;
			break;
		}
        if (!user_only) {
            wq_busy = freeze_workqueues_busy();
            todo += wq_busy;
        }
		if (!todo || time_after(jiffies, end_time))
			break;
        
        if (pm_wakeup_pending()) {
			wakeup = true;
			break;
		}
        
        usleep_range(sleep_usecs / 2, sleep_usecs);
        if (sleep_usecs < 8 * USEC_PER_MSEC)
            sleep_usecs *= 2;
    }

	do_gettimeofday(&end);
	elapsed_msecs64 = timeval_to_ns(&end) - timeval_to_ns(&start);
    do_div(elapsed_msecs64, NSEC_PER_MSEC);
    elapsed_msecs = elapsed_msecs64;

	if (todo) {
		/* This does not unfreeze processes that are already frozen
		 * (we have slightly ugly calling convention in that respect,
		 * and caller must call thaw_processes() if something fails),
		 * but it cleans up leftover PF_FREEZE requests.
		 */
		printk("\n");
		printk(KERN_ERR "Freezing of tasks %s after %d.%03d seconds "
				"(%d tasks refusing to freeze):\n",
				wakeup ? "aborted" : "failed",
               elapsed_msecs / 1000, elapsed_msecs % 1000, todo);
        
        thaw_workqueues();
        
		read_lock(&tasklist_lock);
		do_each_thread(g, p) {
			if (freezing(p) && !freezer_should_skip(p) &&
                elapsed_msecs > 1000 &&
                p != current && freezing(p) && !frozen(p))
				sched_show_task(p);
		} while_each_thread(g, p);
		read_unlock(&tasklist_lock);
	} else {
		printk("(elapsed %d.%03d seconds) ", elapsed_msecs / 1000,
                elapsed_msecs % 1000);
	}

	return todo ? -EBUSY : 0;
}

/**
 *	freeze_processes - tell processes to enter the refrigerator
 */
int freeze_processes(void)
{
	int error;

    if (!pm_freezing)
        atomic_inc(&system_freezing_cnt);
    
	printk("Freezing user space processes ... ");
    pm_freezing = true;
	error = try_to_freeze_tasks(true);
	if (!error) {
        printk("done.");
        oom_killer_disable();
    }
    printk("\n");
    BUG_ON(in_atomic());
    
    if (error)
        thaw_processes();
    return error;
}

/**
 * freeze_kernel_threads - Make freezable kernel threads go to the refrigerator.
 */
int freeze_kernel_threads(void)
{
    int error;

	printk("Freezing remaining freezable tasks ... ");
    pm_nosig_freezing = true;
	error = try_to_freeze_tasks(false);
	if (!error)
        printk("done.");

	printk("\n");
    BUG_ON(in_atomic());
    
    if (error)
        thaw_processes();
	return error;
}

void thaw_processes(void)
{
	struct task_struct *g, *p;
    
	if (pm_freezing)
		atomic_dec(&system_freezing_cnt);
	pm_freezing = false;
	pm_nosig_freezing = false;
    
	oom_killer_enable();
    
	printk("Restarting tasks ... ");
    
	thaw_workqueues();
    
	read_lock(&tasklist_lock);
	do_each_thread(g, p) {
		__thaw_task(p);
	} while_each_thread(g, p);
	read_unlock(&tasklist_lock);
    
	schedule();
	printk("done.\n");
}

void thaw_kernel_threads(void)
{
	struct task_struct *g, *p;
    
	pm_nosig_freezing = false;
	printk("Restarting kernel threads ... ");
    
	thaw_workqueues();
    
	read_lock(&tasklist_lock);
	do_each_thread(g, p) {
		if (p->flags & (PF_KTHREAD | PF_WQ_WORKER))
			__thaw_task(p);
	} while_each_thread(g, p);
	read_unlock(&tasklist_lock);
    
	schedule();
	printk("done.\n");
}
