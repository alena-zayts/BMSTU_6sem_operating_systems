//https://www.kernel.org/doc/Documentation/filesystems/seq_file.txt
//https://lwn.net/Articles/22359/
//https://www.kernel.org/doc/html/latest/filesystems/seq_file.html

// различия: 
// 		открытие (связывает fortune_show с файлом) и закрытие файла
//		сама fortune_show: вместо copy_to_user(buf, tmp, len) испоьзуется seq_printf(m, "%s", tmp) (void)
//						   нет     *f_pos += len; buf += len;

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
Most code will simply use seq_printf(), which works pretty much like
printk(), but which requires the seq_file pointer as an argument.
*/

int fortune_show(struct seq_file* m, void* v) 
{
    int len;
    if (!write_index)
        return 0;
	
	// Кольцевой буфер.
    if (read_index >= write_index)
        read_index = 0;
	
    len = snprintf(tmp, COOKIE_BUF_SIZE, "%s", &cookie_buf[read_index]);
	
    
	seq_printf(m, "%s", tmp); // void
	
    read_index += len + 1;
	
    printk(KERN_DEBUG "+ : Произведено чтение\n");
	//printk(KERN_DEBUG "+ : read_index = %d", read_index);
    return 0;
}


// buf - откуда (пространство пользователя)
// возвращается количество символов, фактически записанных в файл
static ssize_t fortune_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
    int space_available = (COOKIE_BUF_SIZE - write_index) + 1;

    if (space_available < count)
    {
        printk(KERN_DEBUG "+ Ошибка : В буфере недостаточно места!");
        return -ENOSPC; // передается пользовательскому процессу
    }
    
    // куда (пространство ядра), откуда (пространоство пользователя), количество
    // возвращает количество незаписанных символов.
    if (copy_from_user(&cookie_buf[write_index], buf, count))
    {
        printk(KERN_DEBUG "+ Ошибка : copy_from_user!");
        return -EFAULT; // EFAULT - неправильный адрес.
    }

    write_index += count;
    cookie_buf[write_index - 1] = 0;

    printk(KERN_DEBUG "+ : Произведена запись в файл");
	// printk(KERN_DEBUG "+ : write_index = %d", write_index);
    return count;
}

int fortune_open(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "+ Вызван fortune_open\n");
    return single_open(file, fortune_show, NULL);
	/* 
	int single_open(struct file *file,
	                int (*show)(struct seq_file *m, void *p),
	                void *data);
		если использовать seq_open, то потребуется 
		static const struct seq_operations ct_seq_ops = {
	        .start = ct_seq_start,
	        .next  = ct_seq_next,
	        .stop  = ct_seq_stop,
	        .show  = ct_seq_show
	};
	*/
}

int fortune_release(struct inode *inode, struct file *file) 
{
    printk(KERN_DEBUG "+ Вызван fortune_release\n");
    return single_release(inode, file); // так как использовали single_open
}


static struct proc_ops fops =
    {
        // Для операций с файлом будут вызываться наши функции.
        proc_read: seq_read,
        proc_write: fortune_write,
        proc_open: fortune_open,
        proc_release: fortune_release,
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

module_init(fortune_init);
module_exit(fortune_exit);
	
	
/*
make
sudo insmod seqfile.ko
 
dmesg | grep "+ :"
lsmod |grep seqfile
ls /proc | grep seqfile
echo "Msg1" > /proc/seqfile
echo "Msg2" > /proc/seqfile
echo "Msg3" > /proc/seqfile
cat /proc/seqfile
echo "Msg4" > /proc/seqfile
cat /proc/seqfile
 cat /proc/seqfile
 cat /proc/seqfile
 cat /proc/seqfile
 dmesg | grep "+ :"
 
 
 cat /proc/seqfile_symlink
 
 
sudo rmmod seqfile.ko
dmesg | grep "+ :"
lsmod |grep seqfile
ls /proc | grep seqfile

 
 */
