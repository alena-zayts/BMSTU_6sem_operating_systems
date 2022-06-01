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

#define MSG ">> my_tasklet: "
#define MY_TAB "" //"        "
#define WITH_SLEEP false
#define ERR_KLAV false

#define COLOR_START1 "\033[01;34m"
#define COLOR_START2 "\033[01;32m"
#define COLOR_END   "\x1B[0;37;40m"
#define IRQ_NUM 1 // прерывание от клавиатуры

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ALENA");

char *my_tasklet_data = "my_tasklet data";
struct tasklet_struct* my_tasklet;


void my_tasklet_function(unsigned long data)
{
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
        printk(COLOR_START1 MSG MY_TAB "(func): keyboard=%s, state=%lu\n", ascii[code], my_tasklet->state);


    }
    else if (ERR_KLAV)
    {
        printk(COLOR_START2 MSG MY_TAB "(func): unknown code=%d\n", code);
    }

    if (WITH_SLEEP)
    {
        printk(COLOR_START1 MSG MY_TAB "go to sleep at %llu\n", ktime_get());
        mdelay(1000);
        printk(COLOR_START1 MSG MY_TAB "return at      %llu\n", ktime_get());
    }

}




irqreturn_t my_handler(int irq, void *dev) //устаревшее ,struct pt_regs *regs
{
    printk(MSG "(handler) (time=%llu): \n", ktime_get());
    if (irq == IRQ_NUM)
    {
        /*
        Когда tasklet запланирован, ему выставляется состояние TASKLET_STATE_SCHED, и он добавляется в очередь. (на планирование)
        Пока он находится в этом состоянии, запланировать его еще раз не получится, т.е. в этом случае просто ничего не произойдет.
        Tasklet не может находиться сразу в нескольких местах очереди на планирование, которая организуется через поле next структуры tasklet_struct.
        После того, как тасклет был запланирован, он выполниться только один раз.
		(тасклет всегда выполняется на том процессоре, который его запланировал на выполнение)
        */
        printk(MSG "info before scheduling: state=%lu, counter=%u\n", my_tasklet->state, my_tasklet->count.counter);
        tasklet_schedule(my_tasklet);
        printk(MSG "after:                  state=%lu, counter=%u\n", my_tasklet->state, my_tasklet->count.counter);
        return IRQ_HANDLED; // прерывание обработано
    }
    printk(MSG "irq wasn't handled\n");
    return IRQ_NONE; // прерывание не обработано
}



static int __init my_init(void)
{
    //гарантированно получаете непрерывные физические блоки памяти
    //важно, если будет доступен устройством DMA на физически адресной шине (например, PCI).
    my_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL); //Allocate normal kernel ram. May sleep.
    if (!my_tasklet)
    {
        printk(MSG "ERROR kmalloc!\n");
        return -1;
    }

    /*
    Динамическое создание (регистрация) тасклета.
    tasklet_init(t, tasklet_handler, dev) ;   
    DECLARE_TASKLET(name, func, data)

    DECLARE_TASKLET(my_tasklet, tasklet_handler, dev);
    эквив:
    struct tasklet_struct my_tasklet = {NULL, 0, ATOMIC_INIT(0), tasklet_handler, dev} ;
    (для DISABLED ATOMIC_INIT(1))

    создается тасклет  с  именем  my_tasklet ,  который разрешен для  выполнения.
    Функция  tasklet_handler  будет  обработчиком этого  тасклета.
    Значение  параметра dev  передается  в  функцию-обработчик  при   вызове   данной функции.
    */
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
        printk(MSG "ERROR request_irq\n");
        return -1;
    }
    printk(MSG "module loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    //ждет завершения тасклета и удаляет тасклет из очереди на выполнение только в контексте процесса. 
    tasklet_kill(my_tasklet);
    kfree(my_tasklet);
    //освободит линию irq от указанного обработчика.  
    free_irq(IRQ_NUM, &my_handler);
    printk(MSG "module unloaded\n");
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
cat /proc/interrupts | head -n 1 && cat /proc/interrupts| grep my_dev_name
*/






/*
struct tasklet_struct
 {
    struct tasklet_struct *next;   указатель на следующий тасклет в     списке 
    unsigned long state;          состояние тасклета
                                    Enum
                                    {
	                                (0)TASKLET_STATE_SCHED, - запланирован
	                                (1)TASKLET_STATE_RUN - выполняется, блокирует tasklet, что предотвращает исполнение одного и того же tasklet’а на разных CPU.
                                     }

    atomic_t count;               счетчик ссылок. если не равен нулю, то тасклет запрещён и не может выполняться
    void (*func) (unsigned long);  функция-обработчик тасклета
    unsigned long data;  аргумент функции-обработчика тасклета 
);
*/




// Посмотреть инф-ию о обработчике прерывания
// cat /proc/interrupts | head -n 1 && cat /proc/interrupts| grep my_dev_name
// CPUi - число прерываний, полученных i-ым процессорным ядром.

// TODO: В нашем случае при каждом нажатии клавиши увеличивается на 3, почему...?
// TODO: А вот состояние 2 это какое...?
// + флаги не совпадают...





/*
Void synchronize_irq(unsigned int irq); - предназначена для ожидания завершения обработчика прерывания по линии irq, если он выполняется. 
*/


