#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alena");

static int __init md_init(void)
{
	printk(KERN_INFO "+ Init\n");
	
	struct task_struct *task = &init_task;

	do 
	{
		printk(KERN_INFO "+ pid=%d, name=%s,     p_pid=%d, p_name=%s\n", 
			task->pid, task->comm, 
			task->parent->pid, task->parent->comm); 
	} while ((task = next_task(task)) != &init_task);

	printk(KERN_INFO "+ Current: pid=%d, name=%s,     p_pid=%d, p_name=%s\n", 
			current->pid, current->comm, 
			current->parent->pid, current->parent->comm); 
			
	return 0;
}

static void __exit md_exit(void)
{
	printk(KERN_INFO "+ Exit\n");
}

module_init(md_init);
module_exit(md_exit);
