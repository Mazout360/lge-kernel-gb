#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9618593f, "module_layout" },
	{ 0xb276704, "sdio_writeb" },
	{ 0x1c95470e, "sdio_readb" },
	{ 0x11cb8c87, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x235443d3, "complete_and_exit" },
	{ 0x17a142df, "__copy_from_user" },
	{ 0x349cba85, "strchr" },
	{ 0xa90c928a, "param_ops_int" },
	{ 0x98082893, "__copy_to_user" },
	{ 0x97255bdf, "strlen" },
	{ 0xcccca482, "_test_and_clear_bit_le" },
	{ 0xc7ec6c27, "strspn" },
	{ 0x382782f5, "wake_lock_destroy" },
	{ 0x326df9e1, "vfs_stat" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xa695ca4a, "remove_wait_queue" },
	{ 0xb7809395, "sdio_enable_func" },
	{ 0xc7a4fbed, "rtnl_lock" },
	{ 0x9a47f535, "sdio_claim_irq" },
	{ 0x5c8f602c, "skb_clone" },
	{ 0x20000329, "simple_strtoul" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0x94af7d90, "down_interruptible" },
	{ 0x1a41a5ed, "wake_lock" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x6e73cd6b, "filp_close" },
	{ 0x4e830a3e, "strnicmp" },
	{ 0xb3803aaa, "init_timer_key" },
	{ 0x6e5bc9da, "mutex_unlock" },
	{ 0x85df9b6c, "strsep" },
	{ 0x4a37af33, "___dma_single_cpu_to_dev" },
	{ 0xa2e85741, "_spin_lock_irqsave" },
	{ 0x6c1ce5ce, "strcspn" },
	{ 0x4f099aa5, "wake_unlock" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x15351bae, "skb_realloc_headroom" },
	{ 0xb3a307c6, "si_meminfo" },
	{ 0x7d11c268, "jiffies" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x206d8b6, "param_ops_string" },
	{ 0xec8025a2, "netif_rx" },
	{ 0x5b89a27c, "__init_waitqueue_head" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x6e765cdb, "wait_for_completion" },
	{ 0xb4c8d0ac, "kernel_read" },
	{ 0x1d237359, "sdio_writel" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x4a33da2d, "del_timer_sync" },
	{ 0xb5eeb329, "register_early_suspend" },
	{ 0x5f754e5a, "memset" },
	{ 0x650756c7, "netif_rx_ni" },
	{ 0xfdd0e137, "dev_alloc_skb" },
	{ 0x8d3894f2, "_ctype" },
	{ 0xe88ecc21, "__mutex_init" },
	{ 0xea147363, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0xdb3877d, "___dma_single_dev_to_cpu" },
	{ 0x71c90087, "memcmp" },
	{ 0xe9e7909f, "free_netdev" },
	{ 0xf397b9aa, "__tasklet_schedule" },
	{ 0x328a05f1, "strncpy" },
	{ 0xb0cd88fb, "register_netdev" },
	{ 0xd2fbd44f, "dma_free_coherent" },
	{ 0x442604ab, "sdio_readl" },
	{ 0x7e9dfcf2, "wireless_send_event" },
	{ 0x3cfedb3f, "register_pm_notifier" },
	{ 0x84b183ae, "strncmp" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x91caaad4, "skb_push" },
	{ 0xa1cb7699, "mutex_lock" },
	{ 0x1e6d26a8, "strstr" },
	{ 0x24051c6f, "down" },
	{ 0x8e80d4fd, "_spin_unlock_irqrestore" },
	{ 0xa5808bbf, "tasklet_init" },
	{ 0xf8e7c3ad, "mod_timer" },
	{ 0xe1a8727b, "do_wifi_cardetect" },
	{ 0xec7b6d3b, "skb_pull" },
	{ 0x5093fa82, "_clear_bit_le" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0x5659df3a, "dev_kfree_skb_any" },
	{ 0xd79b5a02, "allow_signal" },
	{ 0xc529a64d, "dma_alloc_coherent" },
	{ 0x79730b00, "dev_open" },
	{ 0x61651be, "strcat" },
	{ 0x79ad224b, "tasklet_kill" },
	{ 0x3e276551, "sdio_readsb" },
	{ 0x2a5194ba, "sdio_unregister_driver" },
	{ 0xd67125c8, "sdio_f0_writeb" },
	{ 0x7dceceac, "capable" },
	{ 0x9f984513, "strrchr" },
	{ 0x8e682457, "kmem_cache_alloc" },
	{ 0x59aac18, "sdio_release_irq" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x502c651c, "alloc_netdev_mq" },
	{ 0xe5bca160, "wake_lock_timeout" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x96cc93b5, "eth_type_trans" },
	{ 0xb72d75ee, "sdio_f0_readb" },
	{ 0xd31c08a4, "ether_setup" },
	{ 0x94dc300b, "tegra_ioremap" },
	{ 0x57a6504e, "vsnprintf" },
	{ 0x954cdbb0, "sched_setscheduler" },
	{ 0xd93c7eef, "__wake_up" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0x421ae565, "sdio_memcpy_toio" },
	{ 0x3a978298, "sdio_writew" },
	{ 0x8bc18b61, "add_wait_queue" },
	{ 0x4fd6fa59, "_spin_unlock_bh" },
	{ 0x37a0cba, "kfree" },
	{ 0x2b0935de, "tegra_iounmap" },
	{ 0x9d669763, "memcpy" },
	{ 0x89e9d8e5, "wake_lock_init" },
	{ 0xda5ea696, "_test_and_set_bit_le" },
	{ 0xda6c388d, "prepare_to_wait" },
	{ 0x6c8d5ae8, "__gpio_get_value" },
	{ 0xc38be2b, "up" },
	{ 0x6892088c, "unregister_pm_notifier" },
	{ 0xfbdd8f5b, "finish_wait" },
	{ 0x7e9ebb05, "kernel_thread" },
	{ 0xf94b6d11, "sdio_reset_comm" },
	{ 0xb227ae83, "unregister_early_suspend" },
	{ 0x2d815be4, "unregister_netdev" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0xe5ada986, "complete" },
	{ 0x701d0ebd, "snprintf" },
	{ 0xeafc6cd7, "NvOdmWlanEnable" },
	{ 0x1749fb90, "__netif_schedule" },
	{ 0xaa379b2c, "sdio_readw" },
	{ 0xac8a94dd, "sdio_register_driver" },
	{ 0x6375977, "consume_skb" },
	{ 0x5cb4f037, "sdio_memcpy_fromio" },
	{ 0xf9222695, "sdio_claim_host" },
	{ 0xe9a23a48, "_spin_lock_bh" },
	{ 0xc3a1455f, "skb_put" },
	{ 0x13095525, "param_ops_uint" },
	{ 0x9c15e530, "outer_cache" },
	{ 0x1b28c2d4, "down_timeout" },
	{ 0xfcfec36, "sdio_set_block_size" },
	{ 0x6e720ff2, "rtnl_unlock" },
	{ 0x9e7d6bd0, "__udelay" },
	{ 0xdc43a9c8, "daemonize" },
	{ 0x6995ff72, "sdio_disable_func" },
	{ 0xf143f41c, "sdio_release_host" },
	{ 0xe914e41e, "strcpy" },
	{ 0xcc5c4c05, "filp_open" },
	{ 0x313341a3, "_set_bit_le" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("sdio:c*v02D0d0000*");
MODULE_ALIAS("sdio:c*v02D0d0492*");
MODULE_ALIAS("sdio:c*v02D0d04C6*");
MODULE_ALIAS("sdio:c*v02D0d4329*");
MODULE_ALIAS("sdio:c*v02D0d4319*");
