
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


static char *cookie_buf;
static unsigned read_index;
static unsigned write_index;


static struct proc_dir_entry *proc_entry;

char tmp[256];

static ssize_t fortune_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t fortune_read(struct file *, char __user *, size_t, loff_t *);
int fortune_open(struct inode *, struct file *);
int fortune_release(struct inode *, struct file *);

static struct proc_ops fops =
    {

        proc_read: fortune_read,
        proc_write: fortune_write,
        proc_open: fortune_open,
        proc_release: fortune_release,
};

int fortune_open(struct inode *sp_inode, struct file *sp_file)
{
    printk(KERN_DEBUG "+ Вызван fortune_open\n");
    return 0;
}

int fortune_release(struct inode *sp_node, struct file *sp_file)
{
    printk(KERN_DEBUG "+ Вызван fortune_release\n");
    return 0;
}


static ssize_t fortune_write(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
    int space_available = (COOKIE_BUF_SIZE - write_index) + 1;

    if (space_available < count)
    {
        printk(KERN_DEBUG "+ Ошибка : В буфере недостаточно места!");
        return -ENOSPC;
    }
    

    if (copy_from_user(&cookie_buf[write_index], buf, count))
    {
        printk(KERN_DEBUG "+ Ошибка : copy_from_user!");
        return -EFAULT;
    }

    write_index += count;
    cookie_buf[write_index - 1] = 0;

    printk(KERN_DEBUG "+ : Произведена запись в файл");
    return count;
}


static ssize_t fortune_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos)
{
    int len;

    if (*f_pos > 0)
        return 0;


    if (read_index >= write_index)
        read_index = 0;

    len = 0;

    if (write_index > 0)
    {
        len = sprintf(tmp, "%s\n", &cookie_buf[read_index]);


        if (copy_to_user(buf, tmp, len))
        {
            printk(KERN_DEBUG "+ Ошибка : copy_to_user!");
            return -EFAULT;
        }
        
        buf += len; //?
        read_index += len;
    }

    *f_pos += len;

    printk(KERN_DEBUG "+ : Произведено чтение из файла\n");
    return len;
}

static int __init fortune_init(void)
{

    cookie_buf = (char *) vmalloc(COOKIE_BUF_SIZE);

    if (!cookie_buf)
    {
        printk(KERN_INFO "+ Ошибка. Недостаточно памяти для vmalloc!\n");
        return -ENOMEM;
    }


    memset(cookie_buf, 0, COOKIE_BUF_SIZE);


    proc_entry = proc_create(F_READ_WRITE_FILE_NAME, 0666, NULL, &fops);
    if (!proc_entry)
    {
        vfree(cookie_buf);
        printk(KERN_INFO "+ : Ошибка: не удалось создать файл для чтения и записи в proc!\n");
        return -ENOMEM;
    }
    
    printk(KERN_INFO "+ : Создан файл " F_READ_WRITE_FILE_NAME "!\n");

    read_index = 0;
    write_index = 0;

    

    if (!proc_mkdir(F_DIR_NAME, NULL))
    {
        vfree(cookie_buf);
        printk(KERN_INFO "+ : Ошибка: не удалось создать директорию!\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "+ : Создана директория " F_DIR_NAME "!\n");
    
    

    if (!proc_symlink(F_SYMLINK_NAME, NULL, "/proc/"F_DIR_NAME))
    {
        vfree(cookie_buf);
        printk(KERN_INFO "+ : Ошибка: не удалось создать символьную ссылку!\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "+ : Создана символьная ссылка " F_SYMLINK_NAME "!\n");
    

    printk(KERN_INFO "+ : Модуль загружен в ядро!\n");
    
    return 0;
}

static void __exit fortune_exit(void)
{

    remove_proc_entry(F_READ_WRITE_FILE_NAME, NULL);
    printk(KERN_INFO "+ : Удален файл " F_READ_WRITE_FILE_NAME "!\n");
    remove_proc_entry(F_DIR_NAME, NULL);
    printk(KERN_INFO "+ : Удалена директория " F_DIR_NAME "!\n");
    remove_proc_entry(F_SYMLINK_NAME, NULL);
    printk(KERN_INFO "+ : Удалена символьная ссылка " F_SYMLINK_NAME "!\n");

    if (cookie_buf)
        vfree(cookie_buf);

    printk(KERN_INFO "+ : Модуль выгружен из ядра!\n");
}

module_init(fortune_init);
module_exit(fortune_exit);

/*
make
sudo insmod fortune.ko
 
dmesg | grep "+ :"
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
 dmesg | grep "+ :"
 
 
 cat /proc/fortune_symlink
 
 
sudo rmmod fortune.ko
dmesg | grep "+ :"
lsmod |grep fortune
ls /proc | grep fortune

 
 */
