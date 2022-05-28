#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ALENA");

#define IRQ_NUM 1
static int devID;

char *my_tasklet = "Key was clicked";

void my_tasklet_function(unsigned long data)
{
    printk(KERN_INFO "!my_tasklet: %s\n", (char *)data);
}

/* 
 * count - Счётчик ссылок. если не равен нулю, то тасклет запрещён и не может выполняться
 */
struct tasklet_struct *yakubaTasklet;

irqreturn_t yakubaHandler(int irq, void *dev)
{
    printk(KERN_INFO "yakubaTasklet: scheduling...\n");
    if (irq == IRQ_NUM)
    {
        tasklet_schedule(yakubaTasklet);
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}

static struct proc_dir_entry *file = NULL;

// state - TASKLET_STATE_SCHED (тасклет запланирован к выполнению), TASKLET_STATE_RUN (тасклет выполняется)
int procShow(struct seq_file *filep, void *v)
{
    printk(KERN_INFO "<<<yakubaTasklet: called show\n");
    seq_printf(filep, "State: %ld, Count: %ld, Use_callback: %d, Data: %s", yakubaTasklet->state, yakubaTasklet->count,
               yakubaTasklet->use_callback, (char *)yakubaTasklet->data);
    return 0;
}

int procOpen(struct inode *inode, struct file *fileInner)
{
    printk(KERN_INFO "<<<yakubaTasklet: called open\n");
    return single_open(fileInner, procShow, NULL);
}

int procRelease(struct inode *inode, struct file *fileInner)
{
    printk(KERN_INFO "<<<yakubaTasklet: called release\n");
    return single_release(inode, fileInner);
}

static struct proc_ops fops = {proc_read : seq_read, proc_open : procOpen, proc_release : procRelease};

static int __init yakubaTaskletInit(void)
{
    if (!proc_create("tasklet", 0666, file, &fops))
    {
        printk(KERN_INFO "Cannot proc_create!\n");
        return -1;
    }

    yakubaTasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
    if (!yakubaTasklet)
    {
        printk(KERN_INFO "kmalloc error!");
        return -1;
    }
    tasklet_init(yakubaTasklet, my_tasklet_function, (unsigned long)my_tasklet);

    int ret = request_irq(IRQ_NUM, yakubaHandler, IRQF_SHARED, "yakubaHandler", &devID);
    if (ret)
    {
        printk(KERN_ERR "yakubaTasklet: cannot register yakubaHandler\n");
    }
    else
    {
        printk(KERN_INFO "yakubaTasklet: module loaded\n");
    }
    return ret;
}

static void __exit yakubaTasklet_exit(void)
{
    tasklet_kill(yakubaTasklet);
    kfree(yakubaTasklet);
    free_irq(IRQ_NUM, &devID);
    if (file)
        remove_proc_entry("tasklet", NULL);
    printk(KERN_DEBUG "yakubaTasklet: module unloaded\n");
}

module_init(yakubaTaskletInit) 
module_exit(yakubaTasklet_exit)
