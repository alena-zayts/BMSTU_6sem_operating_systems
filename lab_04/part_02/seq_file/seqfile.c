#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h>  
#include <linux/vmalloc.h>
#include <linux/proc_fs.h> 
#include <linux/uaccess.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alena");
MODULE_DESCRIPTION("Seqfile Kernel Module");

#define COOKIE_BUF_SIZE PAGE_SIZE

#define F_READ_WRITE_FILE_NAME "seqfile"
#define F_SYMLINK_NAME "seqfile_symlink"
#define F_DIR_NAME "seqfile_dir"


static struct proc_dir_entry *fortune_dir;
static struct proc_dir_entry *fortune_file;
static struct proc_dir_entry *fortune_symlink;

static char *cookie_buf;
static int write_index;
static int read_index;

static char tmp[256];


/*
Для фактического вывода чего-то интересного для пространства пользователя ядро вызывает метод show. 
Этот метод должен создать вывод для элемента в последовательности, указанной итератором v. 
Однако, он не должен использовать printk, вместо этого для вывода существует специальный набор функций seq_file:
int seq_printf(struct seq_file *sfile, const char *fmt, ...);
Это эквивалент printf для реализаций seq_file; он принимает обычную строку формата и дополнительные аргументы значений. 
Однако, вы также должны передать ей структуру seq_file, которая передаётся в функцию show. 
Если seq_printf возвращает ненулевое значение, это означает, что буфер заполнен и вывод будет отброшен.
*/

int fortune_show(struct seq_file* m, void* v) {
    int len;
    if (!write_index)
        return 0;
    /*if (read_index >= write_index)*/
        /*read_index = 0;*/
    len = snprintf(tmp, COOKIE_BUF_SIZE, "%s", &cookie_buf[read_index]);
    seq_printf(m, "%s", tmp);
    read_index += len;
    printk(KERN_INFO "+: show is called\n");
    return 0;
}

ssize_t fortune_write(struct file *filp, const char __user *buf, size_t len, loff_t *offp) {
    if (len > COOKIE_BUF_SIZE - write_index + 1) {
        printk(KERN_ERR "+: cookie_buf overflow error\n");
        return -ENOSPC;
    }
    if (copy_from_user(&cookie_buf[write_index], buf, len)) {
        printk(KERN_ERR "+: copy_to_user error\n");
        return -EFAULT;
    }
    printk(KERN_INFO "+: write is called\n");
    write_index += len - 1;
    cookie_buf[write_index] = '\0';
    return len;
}

int fortune_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "+: called open\n");
    return single_open(file, fortune_show, NULL);
}

int fortune_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "+: called release\n");
    return single_release(inode, file);
}

static struct file_operations fops = {
    read: seq_read, 
    write: fortune_write, 
    open: fortune_open, 
    release: fortune_release
};

static void fortune_free(void) 
{
    if (fortune_symlink)
    {
        // имя, расположение
        remove_proc_entry(F_SYMLINK_NAME, NULL);
        printk(KERN_INFO "+ : Удалена символьная ссылка " F_SYMLINK_NAME "!\n");
    }
    
    if (fortune_file)
    {
            remove_proc_entry(F_READ_WRITE_FILE_NAME, NULL);
            printk(KERN_INFO "+ : Удален файл " F_READ_WRITE_FILE_NAME "!\n");
    }
    if (fortune_dir)
    {
        remove_proc_entry(F_DIR_NAME, NULL);
        printk(KERN_INFO "+ : Удалена директория " F_DIR_NAME "!\n");
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
        printk(KERN_INFO "+ Ошибка. Недостаточно памяти для vmalloc!\n");
        return -ENOMEM; 
    }

    // Заполняем нулями выделенную область.
    memset(cookie_buf, 0, COOKIE_BUF_SIZE);

    // Создаем файл в /proc/ на запись и чтение.
    // имя файла, права доступа, место (NULL для корня /proc root), struct file_operations
    // create_proc_entry не поддерживается
    if (!(fortune_file = proc_create(F_READ_WRITE_FILE_NAME, 0666, NULL, &fops)))
    {
        printk(KERN_INFO "+ : Ошибка: не удалось создать файл для чтения и записи в proc!\n");
        fortune_free();
        return -ENOMEM;
    }
    printk(KERN_INFO "+ : Создан файл " F_READ_WRITE_FILE_NAME "!\n");

	write_index = 0;
    read_index = 0;

    
    // имя, место (NULL для корня /proc root)
    if (!(fortune_dir = proc_mkdir(F_DIR_NAME, NULL)))
    {
        printk(KERN_INFO "+ : Ошибка: не удалось создать директорию!\n");
        fortune_free();
        return -ENOMEM;
    }
    printk(KERN_INFO "+ : Создана директория " F_DIR_NAME "!\n");
    
    
    // имя, место (NULL для корня /proc root), на что (на директорию)
    if (!(fortune_symlink = proc_symlink(F_SYMLINK_NAME, NULL, "/proc/"F_DIR_NAME)))
    {
        printk(KERN_INFO "+ : Ошибка: не удалось создать символьную ссылку!\n");
        fortune_free();
        return -ENOMEM;
    }
    printk(KERN_INFO "+ : Создана символьная ссылка " F_SYMLINK_NAME "!\n");
    

    printk(KERN_INFO "+ : Модуль загружен в ядро!\n");
	
    return 0;
}

static void __exit fortune_exit(void) 
{
    fortune_free();
    printk(KERN_INFO "+ : Модуль выгружен из ядра!\n");
}

module_init(fortune_init) 
module_exit(fortune_exit)