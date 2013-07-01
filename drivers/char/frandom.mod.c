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
	{ 0xdd1f2cf4, "cdev_del" },
	{ 0x11cb8c87, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xf70c28c2, "cdev_init" },
	{ 0xa90c928a, "param_ops_int" },
	{ 0x98082893, "__copy_to_user" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0x94af7d90, "down_interruptible" },
	{ 0xcb9f6024, "device_destroy" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x5f754e5a, "memset" },
	{ 0xea147363, "printk" },
	{ 0x331374d6, "device_create" },
	{ 0x46ec1601, "cdev_add" },
	{ 0x8e682457, "kmem_cache_alloc" },
	{ 0x37a0cba, "kfree" },
	{ 0xc38be2b, "up" },
	{ 0xab46bb7d, "class_destroy" },
	{ 0xb009a230, "__class_create" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

