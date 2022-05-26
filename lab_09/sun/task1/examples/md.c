#include <linux/module.h> // MODULE_LICENSE, MODULE_AUTHOR
#include <linux/kernel.h> // KERN_INFO
#include <linux/init.h>	  // ​Макросы __init и ​__exit
#include <linux/init_task.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alice");
MODULE_DESCRIPTION("My first module!");

struct proc_dir_entry *proc_file_entry;

static int f(struct seq_file *m, void *v)
{
	seq_printf(m, "Info\n");
	return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, f, NULL);
}

static const struct file_operations my_proc_fops =
	{
		.open = my_proc_open,
		.read = seq_read,
		.llseek = seq_lseek,
		.release = single_release,
};

static int __init md_init(void)
{
	proc_file_entry = proc_create("proc_file_name", 0, NULL, &my_proc_fops);
	if (proc_file_entry == NULL)
		return -ENOMEM;

	printk(KERN_INFO "Module: module md start!\n");
	return 0;
}

static void __exit md_exit(void)
{
	remove_proc_entry("proc_file_name", NULL);
	printk(KERN_INFO "Module: Goodbye!\n");
}

module_init(md_init);
module_exit(md_exit);