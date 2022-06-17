#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/time.h>
#include <linux/delay.h>

#define IRQ_NUM 1 // прерывание от клавиатуры

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ALENA");

char *my_tasklet_data = "my_tasklet data";
struct tasklet_struct* my_tasklet;


void my_tasklet_function(unsigned long data)
{
    int code = inb(0x60);
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
        printk( ">> my_tasklet: "  "(func): keyboard=%s, state=%lu\n", ascii[code], my_tasklet->state);


    }
}



irqreturn_t my_handler(int irq, void *dev) //устаревшее ,struct pt_regs *regs
{
    printk(">> my_tasklet: " "(handler) (time=%llu): \n", ktime_get());
    if (irq == IRQ_NUM)
    {
        printk(">> my_tasklet: " "info before scheduling: state=%lu, counter=%u\n", my_tasklet->state, my_tasklet->count.counter);
        tasklet_schedule(my_tasklet); // планирование тасклета
        printk(">> my_tasklet: " "after:                  state=%lu, counter=%u\n", my_tasklet->state, my_tasklet->count.counter);
        return IRQ_HANDLED; // прерывание обработано
    }
    printk(">> my_tasklet: " "irq wasn't handled\n");
    return IRQ_NONE; // прерывание не обработано
}



static int __init my_init(void)
{
    //гарантированно получаете непрерывные физические блоки памяти
    //важно, если будет доступен устройством DMA на физически адресной шине (например, PCI).
	//Allocate normal kernel ram. May sleep.
    my_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL); 
    if (!my_tasklet)
    {
        printk(">> my_tasklet: " "ERROR kmalloc!\n");
        return -1;
    }

    //Динамическое создание (регистрация) тасклета.
    tasklet_init(my_tasklet, my_tasklet_function, (unsigned long)my_tasklet_data);



    /*
    регистрация обработчика аппаратного прерывания и разрешение определенной линии irq

    IRQF_SHARED 0x00000080 – устанавливается абонентами  (теми,  кто вызывает), чтобы разрешить разделение линии IRQ раными устройствами.
    (ДРАЙВЕР управляет устройством – разные драйвера устройсств мб заинтересованы в использовании одной и той же линии IRQ).
    Одно устройство может иметь несколько обработчиков прерываний

    4 параметр devname - имя устройства (можно потом посмотреть в /proc/interrupts)
    5 параметр dev_id – используется прежде всего для разделения (shared) линии прерывания
        Данные по указателю dev требуются для удаления только конкретного устройства.
        Указатель void позволяет передавать все, что требуется, например указатель на handler.
        В результате free_irq() освободит линию irq от указанного обработчика.
    */

    if (request_irq(IRQ_NUM, my_handler, IRQF_SHARED, "my_dev_name", &my_handler))
    {
        printk(">> my_tasklet: " "ERROR request_irq\n");
        return -1;
    }
    printk(">> my_tasklet: " "module loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    //ждет завершения тасклета и удаляет тасклет из очереди на выполнение только в контексте процесса. 
    tasklet_kill(my_tasklet);
    kfree(my_tasklet);
    //освободит линию irq от указанного обработчика.  
    free_irq(IRQ_NUM, &my_handler);
    printk(">> my_tasklet: " "module unloaded\n");
}

module_init(my_init) 
module_exit(my_exit)

/*
sudo rmmod my_tasklet.ko
make disclean
make
sudo insmod my_tasklet.ko
lsmod | grep my_tasklet
sudo dmesg | grep my_tasklet:
// Посмотреть инф-ию о обработчике прерывания. Там CPUi - число прерываний, полученных i-ым процессорным ядром.
cat /proc/interrupts | head -n 1 && cat /proc/interrupts| grep my_dev_name
*/


