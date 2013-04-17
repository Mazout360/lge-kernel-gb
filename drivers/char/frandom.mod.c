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
	{ 0xa1f43910, "module_layout" },
	{ 0x4bb01a27, "cdev_del" },
	{ 0xbeef0151, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x26272027, "cdev_init" },
	{ 0x6980fe91, "param_get_int" },
	{ 0x98082893, "__copy_to_user" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0x94af7d90, "down_interruptible" },
	{ 0x2336105e, "device_destroy" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xff964b25, "param_set_int" },
	{ 0x5f754e5a, "memset" },
	{ 0xea147363, "printk" },
	{ 0xd43e9eef, "device_create" },
	{ 0x7aeed279, "cdev_add" },
	{ 0x9aba63a3, "kmem_cache_alloc" },
	{ 0x37a0cba, "kfree" },
	{ 0xc38be2b, "up" },
	{ 0x24ac8527, "class_destroy" },
	{ 0x664d149a, "__class_create" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

