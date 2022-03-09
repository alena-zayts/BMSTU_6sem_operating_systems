#include <linux/init.h>
#include <linux/module.h>

#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alena");

// extern char *md1_noexport(void);
// extern char *md1_local(void);

static int __init md_init(void)
{
	printk("+ module md3 start!\n");
	printk("+ data string exported from md1 : %s\n", md1_data);
	printk("+ string returned md1_proc() is : %s\n", md1_proc());

	//printk("+ Попытка вызвать md1_noexport\n");
	// printk("+ string returned md1_noexport() is : %s\n", md1_noexport());
	//printk("+ md1_noexport был вызван\n");

	// printk("+ Попытка вызвать md1_local\n");
	// printk("+ string returned md1_local() is : %s\n", md1_local());
	// printk("+ md1_local был вызван\n");
	
	return -1;
}

module_init(md_init);
