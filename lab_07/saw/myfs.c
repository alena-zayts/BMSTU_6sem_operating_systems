#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init_task.h>
#include <linux/init.h>
#include<linux/fs.h>
#include<linux/time.h>

#define FS_MAGIC_NUMBER 0xFEE1DEAD
#define SLAB_NAME "threehudredbucks"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yakuba D.");

struct yakubaFSInode
{
    int i_mode;
    unsigned long i_ino;
};

static int cacheCounter = 0;
static int cacheSize = 64;
static struct kmem_cache *cache = NULL;
static struct yakubaFSInode **inodes = NULL;

static struct yakubaFSInode *getCacheInode(void)
{
    if (cacheCounter == cacheSize)
        return NULL;

    inodes[cacheCounter] = kmem_cache_alloc(cache, GFP_KERNEL);

    return inodes[cacheCounter++];
}


static struct inode *yakubaFSMakeInode(struct super_block *sb, int mode)
{
    struct inode *ret = new_inode(sb); // Allocates a new inode for given superblock
    struct yakubaFSInode *inode = NULL;

    if (ret)
    {
        inode_init_owner(ret, NULL, mode); // init uid, gid, mode for new inode
        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        // ctime - inode change time (attributes)
        // mtime - file modification time
        // atime - access time
        
        inode = getCacheInode();

        if (inode != NULL)
        {
            inode->i_mode = ret->i_mode;
            inode->i_ino = ret->i_ino;
        }

        ret->i_private = inode;
    }

    return ret;
}

static void yakubaFSPutSuper(struct super_block *sb)
{
    printk(KERN_DEBUG ">>>Superblock is destroyed!\n");
}

static struct super_operations const yakubaFSSuperOps = {
    .put_super = yakubaFSPutSuper, // Деструктор суперблока
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static int yakubaFSFillSB(struct super_block *sb, void *data, int silent)
{
    struct inode *root = NULL;

    sb->s_blocksize = PAGE_SIZE; // Так как у нас виртуальная файловая система и минимально выделяемый объём памяти - страница
    sb->s_blocksize_bits = PAGE_SHIFT; // Количество битов для сдвига для получения номера соответствующей страницы
    sb->s_magic = FS_MAGIC_NUMBER;
    sb->s_op = &yakubaFSSuperOps;

    root = yakubaFSMakeInode(sb, S_IFDIR | 0755); // S_IFDIR - маска, говорящая о том, что создаём каталог
    if (!root)
    {
        printk(KERN_ERR ">>>Inode allocation error\n");
        return -ENOMEM;
    }
    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;
    sb->s_root = d_make_root(root);

    if (!sb->s_root)
    {
        printk(KERN_ERR ">>>Root creation failed!\n");
        iput(root);
        
        return -ENOMEM;
    }

    return 0;
}

static struct dentry *yakubaFSMount(struct file_system_type *type, int flags, char const *dev, void *data)
{
    struct dentry *const entry = mount_nodev(type, flags, data, yakubaFSFillSB);
    if (IS_ERR(entry))
        printk(KERN_ERR ">>>Mounting failed!\n");
    else
        printk(KERN_DEBUG ">>>FS was successfully mounted.\n");
    
    return entry;
}


static struct file_system_type yakubaFSType = {
    .owner = THIS_MODULE,
    .name = "yakubaFS",
    .mount = yakubaFSMount,
    .kill_sb = kill_litter_super,
};

static int __init yakubaFSInit(void)
{
    int ret = register_filesystem(&yakubaFSType);
    if (ret != 0)
    {
        printk(KERN_ERR ">>>CANNOT REGISTER OWN FILESYSTEM!\n");
        return ret;
    }

    inodes = kmalloc(sizeof(struct yakubaFSInode *) * cacheSize, GFP_KERNEL);
    if (inodes == NULL)
    {
        printk(KERN_ERR ">>>kmalloc for inodes error!\n");
        return -ENOMEM;
    }

    cache = kmem_cache_create(SLAB_NAME, sizeof(struct yakubaFSInode), 0, 0, NULL);
    if (cache == NULL)
    {
        kfree(inodes);
        printk(KERN_ERR ">>>kmem_cache_create problem!\n");
        return -ENOMEM;
    }

    printk(KERN_DEBUG ">>>SUCCESS, YOUR FILESYSTEM IS READY TO GO!\n");
    return 0;
}

static void __exit yakubaFSExit(void)
{
    int i = 0;
    while (i < cacheCounter)
    {
        kmem_cache_free(cache, inodes[i]);
        i++;
    }
    kmem_cache_destroy(cache);
    kfree(inodes);

    int ret = unregister_filesystem(&yakubaFSType);
    if (ret != 0)
        printk(KERN_ERR ">>>CANNOT UNREGISTER FILESYSTEM!\n");

    printk(KERN_DEBUG ">>>MODULE FS UNLOADED!\n");
}

module_init(yakubaFSInit);
module_exit(yakubaFSExit);

// sudo mount -o loop -t yakubaFS ./image ./dir