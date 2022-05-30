#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yakuba D.");

#define IRQ 1
static int devID;

struct workqueue_struct *workQueue;

void queueFunctionF(struct work_struct *work)
{
    printk(KERN_INFO "<<<yakubaQueue: Key was clicked (1 worker) and we sleep\n");
    msleep(10);
}

void queueFunctionS(struct work_struct *work)
{
    printk(KERN_INFO "<<<yakubaQueue: Key was clicked (2 worker)\n");
}

struct work_struct fWork;
struct work_struct sWork;

irqreturn_t handler(int irq, void *dev)
{
    printk(KERN_INFO "<<<yakubaQueue: move work to queue...\n");
    if (irq == IRQ)
    {
        queue_work(workQueue, &fWork);
        queue_work(workQueue, &sWork);
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}

static struct proc_dir_entry *file = NULL;

int procShow(struct seq_file *filep, void *v)
{
    printk(KERN_INFO "<<<yakubaQueue: called show\n");
    seq_printf(filep, "Data of a 1 work: %d", fWork.data);
    seq_printf(filep, "Data of a 2 work: %d", sWork.data);
    return 0;
}

int procOpen(struct inode *inode, struct file *fileInner)
{
    printk(KERN_INFO "<<<yakubaQueue: called open\n");
    return single_open(fileInner, procShow, NULL);
}

int procRelease(struct inode *inode, struct file *fileInner)
{
    printk(KERN_INFO "<<<yakubaQueue: called release\n");
    return single_release(inode, fileInner);
}

static struct proc_ops fops = {proc_read : seq_read, proc_open : procOpen, proc_release : procRelease};

static int __init workQueueInit(void)
{
    int ret = request_irq(IRQ, handler, IRQF_SHARED, "handler", &devID);
    if (ret)
    {
        printk(KERN_ERR "<<<yakubaQueue: handler wasn't registered\n");
        return ret;
    }

    if (!(workQueue = create_workqueue("yakubaQueue")))
    {
        free_irq(IRQ, &devID);
        printk(KERN_INFO "<<<yakubaQueue: workqueue wasn't created");
        return -ENOMEM;
    }

    INIT_WORK(&fWork, queueFunctionF);
    INIT_WORK(&sWork, queueFunctionS);

    if (!proc_create("workqueue", 0666, file, &fops))
    {
        printk(KERN_INFO "<<<yakubaQueue: cannot proc_create!\n");
        return -1;
    }

    printk(KERN_INFO "<<<yakubaQueue: module loaded\n");
    return 0;
}

static void __exit workQueueExit(void)
{
    flush_workqueue(workQueue);
    destroy_workqueue(workQueue);
    free_irq(IRQ, &devID);
    remove_proc_entry("workqueue", NULL);
    printk(KERN_INFO "<<<yakubaQueue: module unloaded\n");
}

module_init(workQueueInit) 
module_exit(workQueueExit)
