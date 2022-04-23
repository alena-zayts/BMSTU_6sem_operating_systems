#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/vmalloc.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alena");
MODULE_DESCRIPTION("Fortune Cookie Kernel Module");

#define COOKIE_BUF_SIZE PAGE_SIZE

#define F_READ_WRITE_FILE_NAME "fortune"
#define F_SYMLINK_NAME "fortune_symlink"
#define F_DIR_NAME "fortune_dir"


// структура proc_dir_entry для файлов и поддиректорий файловой системы /proc
static struct proc_dir_entry *fortune_file;
static struct proc_dir_entry *fortune_dir;
static struct proc_dir_entry *fortune_symlink;

static char *cookie_buf;                // буфер пространства ядра
static unsigned read_index;
static unsigned write_index;

static char tmp[256];      // для sprintf






static ssize_t fortune_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t fortune_read(struct file *, char __user *, size_t, loff_t *);
int fortune_open(struct inode *, struct file *);
int fortune_release(struct inode *, struct file *);

static struct proc_ops fops =
    {
        // Для операций с файлом будут вызываться наши функции.
        proc_read: fortune_read,
        proc_write: fortune_write,
        proc_open: fortune_open,
        proc_release: fortune_release,
};

int fortune_open(struct inode *sp_inode, struct file *sp_file)
{
    printk(KERN_INFO "** open Вызван fortune_open\n");
    return 0;
}

int fortune_release(struct inode *sp_node, struct file *sp_file)
{
    printk(KERN_INFO "** release Вызван fortune_release\n");
    return 0;
}

// buf - откуда (пространство пользователя)
// возвращается количество символов, фактически записанных в файл
static ssize_t fortune_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
    int space_available = (COOKIE_BUF_SIZE - write_index) + 1;

    if (space_available < count)
    {
        printk(KERN_INFO "** Ошибка : В буфере недостаточно места!");
        return -ENOSPC; // передается пользовательскому процессу
    }
    
    // куда (пространство ядра), откуда (пространоство пользователя), количество
    // возвращает количество незаписанных символов.
    if (copy_from_user(&cookie_buf[write_index], buf, count))
    {
        printk(KERN_INFO "** Ошибка : copy_from_user!");
        return -EFAULT; // EFAULT - неправильный адрес.
    }

    write_index += count;
    cookie_buf[write_index - 1] = 0;

    printk(KERN_INFO "** write Произведена запись");
    return count;
}

// buf - куда (пространство пользователя)
// возвращает количество считанных символов
static ssize_t fortune_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos)
{
    int len = 0;
	
	if (!write_index)
        return 0;
	
	// cycling
    if (*f_pos > 0)
        return 0;

    // Кольцевой буфер.
    if (read_index >= write_index)
        read_index = 0;

	len = sprintf(tmp, "%s\n", &cookie_buf[read_index]);

	
	// copy_to_user используем, потому что мы работаем в режиме пользователя
	// Процессы в режиме пользователя имеют виртуальное адресное пространство.
	// Там нет абсолютных адресов. Поэтому происходит преобразование виртуального
	// адреса в физический (поддерживается аппаратно)
	// можем использовать copy_to_user, поскольку объявляемый буфер уже находится в пространстве ядра
	
	// куда (пространство пользователя), откуда (пространоство ядра), количество
	// возвращает количество количество байт, которые не могут быть скопированы
	// copy_to_user(buf, tmp, len);
	if (copy_to_user(buf, tmp, len))
	{
		printk(KERN_INFO "** Ошибка : copy_to_user!");
		return -EFAULT;
	}

	
	read_index += len;

    *f_pos += len;
	buf += len;

    printk(KERN_INFO "** read Произведено чтение\n");
    return len;
}

static void fortune_free(void) 
{
    if (fortune_symlink)
    {
        // имя, расположение
        remove_proc_entry(F_SYMLINK_NAME, NULL);
        printk(KERN_INFO "** Удалена символьная ссылка " F_SYMLINK_NAME "!\n");
    }
    
    if (fortune_file)
    {
            remove_proc_entry(F_READ_WRITE_FILE_NAME, NULL);
            printk(KERN_INFO "** Удален файл " F_READ_WRITE_FILE_NAME "!\n");
    }
    if (fortune_dir)
    {
        remove_proc_entry(F_DIR_NAME, NULL);
        printk(KERN_INFO "** Удалена директория " F_DIR_NAME "!\n");
    }
    
    if (cookie_buf)
        vfree(cookie_buf);
}

static int __init fortune_init(void)
{
    // vmalloc - выделить виртуальный непрерывный блок памяти.
    cookie_buf = (char *) vmalloc(COOKIE_BUF_SIZE);

    if (!cookie_buf)
    {
        printk(KERN_INFO "** Ошибка. Недостаточно памяти для vmalloc!\n");
        return -ENOMEM; 
    }

    // Заполняем нулями выделенную область.
    memset(cookie_buf, 0, COOKIE_BUF_SIZE);

    // Создаем файл в /proc/ на запись и чтение.
    // имя файла, права доступа, место (NULL для корня /proc root), struct file_operations
    // create_proc_entry не поддерживается
    if (!(fortune_file = proc_create(F_READ_WRITE_FILE_NAME, 0666, NULL, &fops)))
    {
        printk(KERN_INFO "** Ошибка: не удалось создать файл для чтения и записи в proc!\n");
        fortune_free();
        return -ENOMEM;
    }
    printk(KERN_INFO "** Создан файл " F_READ_WRITE_FILE_NAME "!\n");

    read_index = 0;
    write_index = 0;

    
    // имя, место (NULL для корня /proc root)
    if (!(fortune_dir = proc_mkdir(F_DIR_NAME, NULL)))
    {
        printk(KERN_INFO "** Ошибка: не удалось создать директорию!\n");
        fortune_free();
        return -ENOMEM;
    }
    printk(KERN_INFO "** Создана директория " F_DIR_NAME "!\n");
    
    
    // имя, место (NULL для корня /proc root), на что (на директорию)
    if (!(fortune_symlink = proc_symlink(F_SYMLINK_NAME, NULL, "/proc/"F_DIR_NAME)))
    {
        printk(KERN_INFO "** Ошибка: не удалось создать символьную ссылку!\n");
        fortune_free();
        return -ENOMEM;
    }
    printk(KERN_INFO "** Создана символьная ссылка " F_SYMLINK_NAME "!\n");
    

    printk(KERN_INFO "** Модуль загружен в ядро!\n");
    
    return 0;
}

static void __exit fortune_exit(void)
{
    fortune_free();
    printk(KERN_INFO "** Модуль выгружен из ядра!\n");
}

module_init(fortune_init);
module_exit(fortune_exit);

/*
make
sudo insmod fortune.ko
 
dmesg | grep "**"
lsmod |grep fortune
ls /proc | grep fortune
echo "Msg1" > /proc/fortune
echo "Msg2" > /proc/fortune
echo "Msg3" > /proc/fortune
cat /proc/fortune
echo "Msg4" > /proc/fortune
cat /proc/fortune
 cat /proc/fortune
 cat /proc/fortune
 cat /proc/fortune
 dmesg | grep "**"
 
 
 cat /proc/fortune_symlink
 
 
sudo rmmod fortune.ko
dmesg | grep "**"
lsmod |grep fortune
ls /proc | grep fortune

 
 */
