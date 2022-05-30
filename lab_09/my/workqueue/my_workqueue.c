#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/time.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ALENA");

#define IRQ_NUM 1

#define MSG ">> my_workqueue3: "

#define COLOR_START1 "\033[01;34m"
#define COLOR_START2 "\033[01;32m"


struct workqueue_struct *my_work_queue;
struct work_struct my_work1;
struct work_struct my_work2;

void my_work_func1(struct work_struct *work)
{
    // получение кода нажатой клавиши клавиатуры

    int code = inb(0x60); //https://dvsav.ru/at-ps2-keyboard/
    char * ascii[84] = 
    {" ", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Backspace", 
     "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Ctrl",
     "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "\"", "'", "Shift (left)", "|", 
     "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Shift (right)", 
     "*", "Alt", "Space", "CapsLock", 
     "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
     "NumLock", "ScrollLock", "Home", "Up", "Page-Up", "-", "Left",
     " ", "Right", "+", "End", "Down", "Page-Down", "Insert", "Delete"};

    if (code < 84) 
    {
        printk(COLOR_START1 MSG "           (func1): keyboard=%s\n", ascii[code]);
    }
    else
    {
        printk(COLOR_START1 MSG "           (func1): unknown code=%d\n", code);
    }
}

void my_work_func2(struct work_struct *work)
{
    printk(COLOR_START2 MSG "           (func2): go to sleep at %llu\n", ktime_get());
    mdelay(100);
    //msleep(10);
    printk(COLOR_START2 MSG "           (func2): return at      %llu\n", ktime_get());
}



irqreturn_t my_handler(int irq, void *dev)
{
    if (irq == IRQ_NUM)
    {
        //добавить работу в очередь работ (назначает работу текущему процессору)
        printk(MSG "add works to queue\n");
        queue_work(my_work_queue, &my_work1);
        queue_work(my_work_queue, &my_work2);

        queue_work_on(0, my_work_queue, &my_work1);
        queue_work_on(1, my_work_queue, &my_work2);

        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}



static int __init my_init(void)
{
    int rc = request_irq(IRQ_NUM, my_handler, IRQF_SHARED, "my_dev_name", &my_handler);
    if (rc)
    {
        printk(MSG "ERROR request_irq\n");
        return rc;
    }

    //if (!(my_work_queue = create_workqueue("my_workqueue")))
    if (!(my_work_queue = alloc_workqueue("my_workqueue", __WQ_LEGACY | WQ_MEM_RECLAIM | WQ_UNBOUND, 1)))
    {
        free_irq(IRQ_NUM, &my_handler);
        printk(MSG "ERROR create_workqueue");
        return -ENOMEM;
    }

    INIT_WORK(&my_work1, my_work_func1);
    INIT_WORK(&my_work2, my_work_func2);


    printk(MSG "module loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    //принудительно завершить глобальную очередь работ ядра 
    flush_workqueue(my_work_queue);
    // удалить
    destroy_workqueue(my_work_queue);
    free_irq(IRQ_NUM, &my_handler);
    printk(MSG "module unloaded\n");
}

module_init(my_init) 
module_exit(my_exit)

/*
sudo rmmod my_workqueue.ko
make disclean
make
sudo insmod my_workqueue.ko
lsmod | grep my_workqueue
sudo dmesg | grep my_workqueue
cat /proc/interrupts | head -n 1 && cat /proc/interrupts| grep my_dev_name


*/
