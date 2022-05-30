#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>

#define MYFS_MAGIC_NUMBER 0x13131313
#define MAX_CACHE_SIZE 128
#define SLAB_NAME "myfs_slab"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alena");

struct myfs_inode {
    int i_mode;
    unsigned long i_ino;
};

static struct kmem_cache *inode_cache = NULL;
static struct myfs_inode **inode_pointers = NULL;
static int cached_count = 0;

static struct myfs_inode *cache_get_inode(void) 
{
    if (cached_count == MAX_CACHE_SIZE)
    {
        return NULL;
    }

    return inode_pointers[cached_count++] = kmem_cache_alloc(inode_cache, GFP_KERNEL);
}

// вызывается в myfs_fill_sb (при монтировании), создает корневой каталог файловой системы.
// аргумент mode задает разрешения на создаваемый файл и его тип (маска S_IFDIR говорит функции, что мы создаем каталог).
static struct inode *myfs_make_inode(struct super_block *sb, int mode) 
{
    struct inode *ret = new_inode(sb);
    struct myfs_inode *inode_cache = NULL; //?

    if (ret) 
    {
        inode_init_owner(&init_user_ns, ret, NULL, mode); //?

        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);

        // ?
        if ((inode_cache = cache_get_inode()) != NULL) 
        {
            inode_cache->i_mode = ret->i_mode;
            inode_cache->i_ino = ret->i_ino;
        }

        ret->i_private = inode_cache;
    }

    return ret;
}

// деструктор суперблока разрабатываемой файловой системы.
// будет вызвана внутри kill_block_super (задаем в myfs_type) перед уничтожением структуры super_block, т.е. при размонтировании файловой системы.

static void myfs_put_super(struct super_block *sb) 
{
    printk(KERN_DEBUG "MYFS: super block destroyed\n");
}

// операции струткуры super_block (myfs_put_super и заглушки из libfs)
static struct super_operations const myfs_super_ops = 
{
      .put_super = myfs_put_super,
      .statfs = simple_statfs,
      .drop_inode = generic_delete_inode,
};

// будет вызвана из myfs_mount -> mount_bdev, чтобы проинициализировать суперблок.
// корневой каталог создает именно функция-параметр myfs_fill_sb()
static int myfs_fill_sb(struct super_block *sb, void *data, int silent) 
{
    // заполнение struct super_block
    sb->s_blocksize = PAGE_SIZE; /* размер блока в байтах */
    sb->s_blocksize_bits = PAGE_SHIFT; /* размер блока в битах */
    sb->s_magic = MYFS_MAGIC_NUMBER; //магическое число, по которому драйвер файловой системы может проверить, что на диске хранится именно та самая файловая система, а не что-то еще или прочие данные;
    sb->s_op = &myfs_super_ops; //операции для суперблока

    // создает корневой каталог файловой системы.
    // аргумент mode задает разрешения на создаваемый файл и его тип (маска S_IFDIR говорит функции, что мы создаем каталог).
    struct inode *root = myfs_make_inode(sb, S_IFDIR | 0755);

    if (!root) 
    {
        printk(KERN_ERR "MYFS: inode allocation failed\n");
        return -ENOMEM;
    }

    //Файловые и inode-операции, которые мы назначаем новому inode, взяты из libfs, т.е. предоставляются ядром.
    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;

    // для корневого каталога создается структура dentry, через которую он помещается в directory-кэш.
    // суперблок имеет специальное поле, хранящее указатель на dentry корневого каталога, которое также устанавливается myfs_fill_sb

    if (!(sb->s_root = d_make_root(root)))
    {
        printk(KERN_ERR "MYFS: root creation failed\n");
        iput(root);

        return -ENOMEM;
    }

    return 0;
}

// должна примонтировать устройство и вернуть структуру, описывающую корневой каталог файловой системы
static struct dentry *myfs_mount(struct file_system_type *type, int flags,
                                  char const *dev, void *data) 
{
    // корневой каталог создает именно функция-параметр myfs_fill_sb() (см выше)
    struct dentry *const entry = mount_nodev(type, flags, data, myfs_fill_sb);

    if (IS_ERR(entry))
    {
        printk(KERN_ERR "MYFS: mounting failed\n");
    }
    else
    {
        printk(KERN_DEBUG "MYFS: mounted!\n");
    }

    return entry;
}

static struct file_system_type myfs_type = {
    .owner = THIS_MODULE, //отвечает за счетчик ссылок на модуль, чтобы его нельзя было случайно выгрузить.
    .name = "myfs", //название будет использоваться при ее монтировании.
    .mount = myfs_mount, //будет вызвана при монтировании файловой системы, Когда делается запрос 
    .kill_sb = kill_anon_super, //при размонтировании. ?kill_block_super

};

static int __init myfs_init(void) 
{
    int ret = register_filesystem(&myfs_type);

    if (ret != 0) 
    {
        printk(KERN_ERR "MYFS: Failed to register filesystem\n");
        return ret;
    }

//?
    if ((inode_pointers = kmalloc(sizeof(struct myfs_inode *) * MAX_CACHE_SIZE, GFP_KERNEL)) == NULL)
    {
        printk(KERN_ERR "MYFS: Failed to allocate memory\n");
        return -ENOMEM;
    }

    if ((inode_cache = kmem_cache_create(SLAB_NAME, sizeof(struct myfs_inode), 0, 0, NULL)) == NULL)
    {
        kfree(inode_pointers);
        printk(KERN_ERR "MYFS: Failed to create cache\n");

        return -ENOMEM;
    }

    printk(KERN_DEBUG "MYFS: Module loaded\n");
    return 0;
}

static void __exit myfs_exit(void) 
{
    int i;
    for (i = 0; i < cached_count; i++) 
    {
        kmem_cache_free(inode_cache, inode_pointers[i]);
    }

    kmem_cache_destroy(inode_cache);
    kfree(inode_pointers);

    int ret = unregister_filesystem(&myfs_type);
    if (ret != 0)
    {
        printk(KERN_ERR "MYFS: Can't unregister filesystem\n");
        return;
    }

    printk(KERN_DEBUG "MYFS: Module unloaded\n");
}

module_init(myfs_init);
module_exit(myfs_exit);

// sudo mount -o loop -t myfs ./image ./dir