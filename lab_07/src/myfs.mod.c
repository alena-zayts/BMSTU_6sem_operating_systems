#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x8e6402a9, "module_layout" },
	{ 0x27480d4, "simple_statfs" },
	{ 0x279d41a1, "generic_delete_inode" },
	{ 0x1680395c, "kill_anon_super" },
	{ 0xc11a01b, "unregister_filesystem" },
	{ 0x1e1076f8, "kmem_cache_destroy" },
	{ 0xfe22de5c, "kmem_cache_free" },
	{ 0x37a0cba, "kfree" },
	{ 0x78ed7000, "kmem_cache_create" },
	{ 0xe78dfe6d, "kmem_cache_alloc_trace" },
	{ 0x595451f1, "kmalloc_caches" },
	{ 0x7dda38d0, "register_filesystem" },
	{ 0x4e1272a2, "iput" },
	{ 0xedc32415, "kmem_cache_alloc" },
	{ 0xe9bccd4a, "d_make_root" },
	{ 0xc1be03ea, "simple_dir_operations" },
	{ 0xd8646e08, "simple_dir_inode_operations" },
	{ 0xbabad2cb, "current_time" },
	{ 0x7a75ec73, "inode_init_owner" },
	{ 0xbfa7c6da, "init_user_ns" },
	{ 0xfff71476, "new_inode" },
	{ 0x4c7fdca, "mount_nodev" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6811E745F190ABB0C4BE2B4");
