#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/unistd.h>
#include <linux/fcntl.h>
//#include <unistd.h>

///////
typedef unsigned long byte4_t;
typedef unsigned long byte1_t;
#define MYRWFS_BLOCK_SIZE_BYTES PAGE_SIZE
#define MYRWFS_FILENAME_LEN NAME_MAX //PATH_MAX 
#define MYRWFS_DATA_BLOCK_CNT 64

///////


typedef struct myrwfs_super_block
{
    byte4_t magic_number; /* Magic number - идентифицировать файловую систему */
    byte4_t block_size_bytes; /* размер блока в байтах */
    byte4_t bdev_size_blocks; /* размер устройства в блоках */
    byte4_t entry_size_bytes; /* размер дескриптора файла в байтах */
    byte4_t entry_table_size_blocks; /* размер таблицы дескрипторов в блоках */
    byte4_t entry_table_block_start; /* смещение к началу таблицы дескрипторов в блоках - она расположена после суперблока на устройстве */
    byte4_t entry_count; /* количество дескрипторов в таблице дескрипторов */
    byte4_t data_block_start; /* смещение к началу данных файловой системы на блочном устройстве */
    byte4_t reserved[MYRWFS_BLOCK_SIZE_BYTES / 4 - 8];
} myrwfs_super_block_t;

typedef struct myrwfs_file_entry
{
    char name[MYRWFS_FILENAME_LEN + 1];
    byte4_t size; /* размер дескриптора файла в байтах */
    byte4_t perms; /* разрешения */
    byte4_t blocks[MYRWFS_DATA_BLOCK_CNT];
} myrwfs_file_entry_t;

typedef struct myrwfs_info
{
    struct super_block *vfs_sb; /* обратный указатель на структуру суперблок пространства ядра */
    myrwfs_super_block_t myrwfs_sb; /* указатель на структуру суперблок, расположенную на устройстве */
    byte1_t *used_blocks; /* указатель на количество блоков, помеченных как занятые */
} myrwfs_info_t;


#define ROOT_INODE_NUM (1)
#define MYRWFS_TO_VFS_INODE_NUM(i) (ROOT_INODE_NUM + 1 + (i))
#define VFS_TO_MYRWFS_INODE_NUM(i) ((i) - (ROOT_INODE_NUM + 1))
// источник: https://sysplay.in/blog/linux-device-drivers/2014/07/file-systems-the-semester-project/

#define MYRWFS_ENTRY_FRACTION 0.10 /* 10% блоков резервируем под таблицу дескрипторов - эмпирически */

#define MYRWFS_ENTRY_TABLE_BLOCK_START 1

void write_super_block(int fd, myrwfs_super_block_t *myrwfs_sb) {
    write(fd, myrwfs_sb, sizeof(myrwfs_super_block_t));
}
void write_file_entries_table(int fd, myrwfs_super_block_t *myrwfs_sb) {
    byte1_t block[MYRWFS_BLOCK_SIZE_BYTES];

    for (int i = 0; i < myrwfs_sb->block_size_bytes / myrwfs_sb->entry_size_bytes; i++) {
        memcpy(block + i * myrwfs_sb->entry_size_bytes, &fe, sizeof(fe));
    }

    for (int i = 0; i < myrwfs_sb->entry_table_size_blocks; i++) {
        write(fd, block, sizeof(block));
    }
}

static int read_sb_from_myrwfs(myrwfs_info_t *info, myrwfs_super_block_t *myrwfs_sb) {
    struct buffer_head *bh;
    if (!(bh = sb_bread(info->vfs_sb, 0))) {
        return -1;
    }
    memcpy(myrwfs_sb, bh->b_data, MYRWFS_BLOCK_SIZE_BYTES);
    brelse(bh);
    return 0;
}

static int read_entry_from_myrwfs(myrwfs_info_t *info, int ino, myrwfs_file_entry_t *fe) {
    byte4_t offset = ino * sizeof(myrwfs_file_entry_t);
    byte4_t len = sizeof(myrwfs_file_entry_t);
    byte4_t block = info->myrwfs_sb.entry_table_block_start;
    byte4_t block_size_bytes = info->myrwfs_sb.block_size_bytes;
    byte4_t bd_block_size = info->vfs_sb->s_bdev->bd_block_size;
    byte4_t abs;
    struct buffer_head *bh;

    abs = block * block_size_bytes + offset;
    block = abs / bd_block_size;
    offset = abs % bd_block_size;
    if (offset + len > bd_block_size) {
        return -1;
    }

    if (!(bh = sb_bread(info->vfs_sb, block))) {
        return -1;
    }

    memcpy((void *) fe, bh->b_data + offset, len);
    brelse(bh);

    return 0;
}

static int write_entry_to_myrwfs(myrwfs_info_t *info, int ino, myrwfs_file_entry_t *fe) {
    byte4_t offset = ino * sizeof(myrwfs_file_entry_t);
    byte4_t len = sizeof(myrwfs_file_entry_t);
    byte4_t block = info->myrwfs_sb.entry_table_block_start;
    byte4_t block_size_bytes = info->myrwfs_sb.block_size_bytes;
    byte4_t bd_block_size = info->vfs_sb->s_bdev->bd_block_size;
    struct buffer_head *bh;

    byte4_t abs = block * block_size_bytes + offset;
    block = abs / bd_block_size;
    offset = abs % bd_block_size;
    if (offset + len > bd_block_size) {
        return -1;
    }

    if (!(bh = sb_bread(info->vfs_sb, block))) {
        return -1;
    }

    memcpy(bh->b_data + offset, (void *) fe, len);
    mark_buffer_dirty(bh);
    brelse(bh);
    return 0;
}



int myrwfs_create(myrwfs_info_t *info, char *fn, int perms, myrwfs_file_entry_t *fe) {
    int ino, free_ino, i;

    free_ino = -1;
    for (ino = 0; ino < info->myrwfs_sb.entry_count; ino++) {
       
        if (read_entry_from_myrwfs(info, ino, fe) < 0)
            return -1;
       
        if (!fe->name[0]) {
            free_ino = ino;
            break;
        }
    }
   
    if (free_ino == -1) {
        printk(KERN_ERR "No entries left\n");
        return -1;
    }

    strncpy(fe->name, fn, MYRWFS_FILENAME_LEN);
    fe->name[MYRWFS_FILENAME_LEN] = 0;
    fe->size = 0;
    fe->perms = perms;
    for (i = 0; i < MYRWFS_DATA_BLOCK_CNT; i++) {
        fe->blocks[i] = 0;
    }

    if (write_entry_to_myrwfs(info, free_ino, fe) < 0)
        return -1;

    return MYRWFS_TO_VFS_INODE_NUM(free_ino);
}

int myrwfs_lookup(myrwfs_info_t *info, char *fn, myrwfs_file_entry_t *fe) {
    for (int ino = 0; ino < info->myrwfs_sb.entry_count; ino++) {
       
        if (read_entry_from_myrwfs(info, ino, fe) < 0)
            return -1;
       
        if (!fe->name[0])
            continue;
       
        if (strcmp(fe->name, fn) == 0)
            return MYRWFS_TO_VFS_INODE_NUM(ino);
    }

    return -1;
}


int fill_myrwfs_info(myrwfs_info_t *info) {

    if (read_sb_from_myrwfs(info, &info->myrwfs_sb) < 0) {
        return -1;
    }

    if (info->myrwfs_sb.magic_number != MYRWFS_MAGIC_NUMBER) {
        printk(KERN_ERR "Invalid MYRWFS detected. Giving up.\n");
        return -1;
    }

    byte1_t *used_blocks = (byte1_t *)(vmalloc(info->myrwfs_sb.bdev_size_blocks));
    if (!used_blocks) {
        return -ENOMEM;
    }

    int i;
    for (i = 0; i < info->myrwfs_sb.data_block_start; i++) {
        used_blocks[i] = 1;
    }
    for (; i < info->myrwfs_sb.bdev_size_blocks; i++) {
        used_blocks[i] = 0;
    }

    myrwfs_file_entry_t fe;
    for (int i = 0; i < info->myrwfs_sb.entry_count; i++) {
        if (read_entry_from_myrwfs(info, i, &fe) < 0) {
            vfree(used_blocks);
            return -1;
        }
       
        if (!fe.name[0])
            continue;
       
        for (int j = 0; j < MYRWFS_DATA_BLOCK_CNT; j++) {
            if (fe.blocks[j] == 0) break;
            used_blocks[fe.blocks[j]] = 1;
        }
    }

    info->used_blocks = used_blocks;
    info->vfs_sb->s_fs_info = info;
    return 0;
}

void kill_mywfs_info(myrwfs_info_t *info) {
    if (info->used_blocks)
        vfree(info->used_blocks);
}


int myrwfs_get_data_block(myrwfs_info_t *info) {
    int i;

    for (i = info->myrwfs_sb.data_block_start; i < info->myrwfs_sb.bdev_size_blocks; i++) {
        if (info->used_blocks[i] == 0) {
            info->used_blocks[i] = 1;
            return i;
        }
    }
    return -1;
}

void myrwfs_put_data_block(myrwfs_info_t *info, int i) {
    info->used_blocks[i] = 0;
}





static int myrwfs_fill_super(struct super_block *sb, void *data, int silent) {
   
    printk(KERN_INFO "** MYRWFS: myrwfs_fill_super\n");
   
    myrwfs_info_t *info;
    if (!(info = (myrwfs_info_t *)(kzalloc(sizeof(myrwfs_info_t), GFP_KERNEL))))
        return -ENOMEM;

    info->vfs_sb = sb;
    if (fill_myrwfs_info(info) < 0) {
        kfree(info);
        return -1;
    }
   
    sb->s_magic = info->myrwfs_sb.magic_number;
    sb->s_blocksize = info->myrwfs_sb.block_size_bytes;
    sb->s_blocksize_bits = log_base_2(info->myrwfs_sb.block_size_bytes);
    sb->s_type = &myrwfs;
    sb->s_op = &myrwfs_sops;

    myrwfs_root_inode = iget_locked(sb, ROOT_INODE_NUM);
    if (!myrwfs_root_inode) {
        kill_mywfs_info(info);
        kfree(info);
        return -1;
    }

    if (myrwfs_root_inode->i_state & I_NEW) {
        myrwfs_root_inode->i_op = &myrwfs_iops;
        myrwfs_root_inode->i_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
        myrwfs_root_inode->i_fop = &myrwfs_dops;
        unlock_new_inode(myrwfs_root_inode);
    }

    sb->s_root = d_make_root(myrwfs_root_inode);
    if (!sb->s_root) {
        iget_failed(myrwfs_root_inode);
        kill_mywfs_info(info);
        kfree(info);
        return -ENOMEM;
    }

    return 0;
}



static void myrwfs_put_super(struct super_block *sb) {
    myrwfs_info_t *info = (myrwfs_info_t *)(sb->s_fs_info);

    printk(KERN_INFO "** MYRWFS: myrwfs_put_super\n");
    if (info) {
        kill_mywfs_info(info);
        kfree(info);
        sb->s_fs_info = NULL;
    }
}


static struct dentry *myrwfs_mount(struct file_system_type *fs, int flags, const char *devname, void *data) {
    printk(KERN_INFO "** MYRWFS: myrwfs_mount: devname = %s\n", devname);
    return mount_bdev(fs, flags, devname, data, &myrwfs_fill_super);
}

static struct file_system_type myrwfs = {
    name: "myrwfs",
    fs_flags: FS_REQUIRES_DEV,
    mount:  myrwfs_mount,
    kill_sb: kill_block_super,
    owner: THIS_MODULE
};

static int myrwfs_inode_create(struct inode *parent_inode, struct dentry *dentry, umode_t mode, bool excl) {
    char fn[dentry->d_name.len + 1];
    int perms = 0;
    myrwfs_info_t *info = (myrwfs_info_t *)(parent_inode->i_sb->s_fs_info);
    int ino;
    struct inode *file_inode;
    myrwfs_file_entry_t fe;

    printk(KERN_INFO "** MYRWFS: myrwfs_inode_create\n");

    strncpy(fn, dentry->d_name.name, dentry->d_name.len);
    fn[dentry->d_name.len] = 0;
    if (mode & (S_IRUSR | S_IRGRP | S_IROTH))
        mode |= (S_IRUSR | S_IRGRP | S_IROTH);
    if (mode & (S_IWUSR | S_IWGRP | S_IWOTH))
        mode |= (S_IWUSR | S_IWGRP | S_IWOTH);
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        mode |= (S_IXUSR | S_IXGRP | S_IXOTH);
    perms |= (mode & S_IRUSR) ? 4 : 0;
    perms |= (mode & S_IWUSR) ? 2 : 0;
    perms |= (mode & S_IXUSR) ? 1 : 0;
    if ((ino = myrwfs_create(info, fn, perms, &fe)) == -1)
        return -1;

    file_inode = new_inode(parent_inode->i_sb);
    if (!file_inode)
    {
        myrwfs_remove(info, fn);
        return -ENOMEM;
    }
    printk(KERN_INFO "** MYRWFS: Created new VFS inode for #%d, let's fill in\n", ino);
    file_inode->i_ino = ino;
    file_inode->i_size = fe.size;
    file_inode->i_mode = S_IFREG | mode;
    file_inode->i_fop = &myrwfs_fops;
    if (insert_inode_locked(file_inode) < 0)
    {
        make_bad_inode(file_inode);
        iput(file_inode);
        myrwfs_remove(info, fn);
        return -1;
    }
    d_instantiate(dentry, file_inode);
    unlock_new_inode(file_inode);

    return 0;
}

static struct dentry *myrwfs_inode_lookup(struct inode *parent_inode, struct dentry *dentry, unsigned int flags) {
    myrwfs_info_t *info = (myrwfs_info_t *)(parent_inode->i_sb->s_fs_info);
    char fn[dentry->d_name.len + 1];
    int ino;
    myrwfs_file_entry_t fe;
    struct inode *file_inode = NULL;

    printk(KERN_INFO "** MYRWFS: myrwfs_inode_lookup\n");

    if (parent_inode->i_ino != myrwfs_root_inode->i_ino)
        return ERR_PTR(-ENOENT);z

    strncpy(fn, dentry->d_name.name, dentry->d_name.len);
    fn[dentry->d_name.len] = 0;
    if ((ino = myrwfs_lookup(info, fn, &fe)) == -1)
      return d_splice_alias(file_inode, dentry);

    file_inode = iget_locked(parent_inode->i_sb, ino);
    if (!file_inode)
        return ERR_PTR(-EACCES);

    if (file_inode->i_state & I_NEW) {
        file_inode->i_size = fe.size;
        file_inode->i_mode = S_IFREG;
        file_inode->i_mode |= ((fe.perms & 4) ? S_IRUSR | S_IRGRP | S_IROTH : 0);
        file_inode->i_mode |= ((fe.perms & 2) ? S_IWUSR | S_IWGRP | S_IWOTH : 0);
        file_inode->i_mode |= ((fe.perms & 1) ? S_IXUSR | S_IXGRP | S_IXOTH : 0);
        file_inode->i_fop = &myrwfs_fops;
        unlock_new_inode(file_inode);
    }

    d_add(dentry, file_inode);
    return NULL;
}

static struct inode_operations myrwfs_iops = {
    create: myrwfs_inode_create,
    lookup: myrwfs_inode_lookup
};






















#define MYFS_MAGIC_NUMBER 0x13131313
#define MAX_CACHE_SIZE 128
#define SLAB_NAME "myfs_slab"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alena");

struct myfs_inode {
    int i_mode;
    unsigned long i_ino;
};


static struct kmem_cache *my_inode_cache = NULL; //создание структуры кэша slab
static struct myfs_inode **inode_pointers = NULL;
static int cached_count = 0;

static struct myfs_inode *cache_get_inode(void) 
{
    printk(KERN_DEBUG "MYFS: ! in cache_get_inode\n");

    if (cached_count == MAX_CACHE_SIZE)
    {
        printk(KERN_DEBUG "MYFS: reached MAX_CACHE_SIZE\n");
        return NULL;
    }

    //После того, как кэш объектов создан, вы можете выделять объекты из него, вызывая функцию kmem_cache_alloc().
    // Вызывающий код передает кэш, из которого выделяется объект, и набор флагов
    return inode_pointers[cached_count++] = kmem_cache_alloc(my_inode_cache, GFP_KERNEL);
}

// вызывается в myfs_fill_sb (при монтировании), создает корневой каталог файловой системы.
// аргумент mode задает разрешения на создаваемый файл и его тип (маска S_IFDIR говорит функции, что мы создаем каталог).

static struct inode *myfs_make_inode(struct super_block *sb, int mode) 
{
    struct inode *ret = new_inode(sb);
    struct myfs_inode *my_inode_cache = NULL; 

    if (ret) 
    {
        // init_user-ns - namespace
        inode_init_owner(&init_user_ns, ret, NULL, mode);

        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);

        if ((my_inode_cache = cache_get_inode()) != NULL) 
        {
            my_inode_cache->i_mode = ret->i_mode;
            my_inode_cache->i_ino = ret->i_ino;
        }

        ret->i_private = my_inode_cache; //?
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
//В поле put_super -- функция удаления суперблока, statfs -- получения информации о файле, drop_inode -- для удаления дисковой копии inode.
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

    /*
    kill_block_super(), which unmounts a file system on a block device
    kill_anon_super(), which unmounts a virtual file system (information is generated when requested)
    */
};

static int __init myfs_init(void) 
{
    int ret = register_filesystem(&myfs_type);

    if (ret != 0) 
    {
        printk(KERN_ERR "MYFS: Failed to register filesystem\n");
        return ret;
    }

    if ((inode_pointers = kmalloc(sizeof(struct myfs_inode *) * MAX_CACHE_SIZE, GFP_KERNEL)) == NULL)
    {
        printk(KERN_ERR "MYFS: Failed to allocate inode_pointers\n");
        return -ENOMEM;
    }

    /* создания нового кэша. Обычно это происходит во время инициализации ядра или при первой загрузке модуля ядра.  
        name — строка имени кэша;
        size — размер элементов кэша (единый и общий для всех элементов);
        offset — смещение первого элемента от начала кэша (для обеспечения соответствующего выравнивания по границам страниц, достаточно указать 0, что означает выравнивание по умолчанию);
        flags — опциональные параметры (может быть 0);
        ctor, dtor — конструктор и деструктор, соответственно, вызываются при размещении-освобождении каждого элемента, но с некоторыми ограничениями ... например, деструктор будет вызываться (финализация), но не гарантируется, что это будет поисходить сразу непосредственно после удаления объекта.
        эта функция не выделяет память кешу. Вместо этого при попытке выделить объекты из кэша (изначально он пуст) ему выделяется память при помощи команды refill.
        Это та же команда, которая используется для добавления кэшу памяти, когда все его объекты израсходованы.
    */
    if ((my_inode_cache = kmem_cache_create(SLAB_NAME, sizeof(struct myfs_inode), 0, 0, NULL)) == NULL)
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
        kmem_cache_free(my_inode_cache, inode_pointers[i]);
    }

    // уничтожение слаба
    // Операция уничтожения может быть успешна, только если уже все объекты,
    // полученные из кэша, были возвращены в него.
    kmem_cache_destroy(my_inode_cache); 
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
