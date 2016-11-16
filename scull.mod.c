#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0xd9f52cf9, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x8eb979b3, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x959d7a2e, __VMLINUX_SYMBOL_STR(param_ops_charp) },
	{ 0x818498, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0xf9c69d29, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x5c7fcba3, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x2b31d71, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x39ed567d, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{        0, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{        0, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{        0, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x78e739aa, __VMLINUX_SYMBOL_STR(up) },
	{ 0x6dc0c9dc, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0xe49e3e2b, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x3938a11a, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{        0, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "DB5424D4B020F2D03D57713");
