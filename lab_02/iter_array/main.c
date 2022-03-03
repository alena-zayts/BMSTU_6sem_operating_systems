#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "stack.h"

#define FTW_F 1 
#define FTW_D 2 
#define FTW_DNR 3 
#define FTW_NS 4

#define RESET_COLOR "\033[0m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define BLUE "\033[0;36m"
#define YELLOW "\033[0;33m"

typedef int Myfunc(const char *, const struct stat *, int, int);

static Myfunc increment_stats;
static int my_ftw(char *, Myfunc * );
int dopath(Myfunc *func, char *fullpath, int depth);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;
static struct stack stack_t;

void printStats()
{
   ntot = nreg + ndir +  nblk + nchr +  nfifo + nslink + nsock;

    if (ntot == 0)
		ntot = 1;

    printf(BLUE);
	printf("_______________________________\nСтатистика:\n\n");
    printf("Обычные файлы:                          %-7ld %-5.2f %%\n", nreg, nreg * 100.0 / ntot);
    printf("Каталоги:                               %-7ld %-5.2f %%\n", ndir, ndir * 100.0 / ntot);
    printf("Специальные файлы блочных устройств:    %-7ld %-5.2f %%\n", nblk, nblk * 100.0 / ntot);
    printf("Специальные файлы символьных устройств: %-7ld %-5.2f %%\n", nchr, nchr * 100.0 / ntot);
    printf("FIFO:                                   %-7ld %-5.2f %%\n", nfifo, nfifo * 100.0 / ntot);
    printf("Символические ссылки:                   %-7ld %-5.2f %%\n", nslink, nslink * 100.0 / ntot);
    printf("Сокеты:                                 %-7ld %-5.2f %%\n\n", nsock, nsock * 100.0 / ntot);
    printf("Всего:                                  %-7ld\n", ntot);
    printf(RESET_COLOR);
}

static int increment_stats(const char *pathname, const struct stat * statptr, int type, int depth)
{
	printf(PURPLE);
    for (int i = 0; i < depth; i++)
        printf("    │");
	
	switch(type)
    {
        case FTW_F: 
            printf(YELLOW);
            switch(statptr->st_mode & S_IFMT) 
            {
                case S_IFREG: nreg++; break;
                case S_IFBLK: nblk++; break;
                case S_IFCHR: nchr++; break;
                case S_IFIFO: nfifo++; break;
                case S_IFLNK: nslink++; break;
                case S_IFSOCK: nsock++; break;
                case S_IFDIR: 
                    perror("Католог имеет тип FTW_F"); 
					return(-1);
            }
            break;
        case FTW_D: 
            ndir++; break;
        case FTW_DNR:
            printf("Закрыт доступ к каталогу %s!", pathname); 
			return(-1);
        case FTW_NS:
            printf("Ошибка функции stat для %s!", pathname); 
			return(-1);
        default: 
            printf("Неизвестый тип %d для файла %s!", type, pathname); 
			return(-1);
    }

    printf("    ├── %s     inode=%d %s\n", pathname, (int) statptr->st_ino, RESET_COLOR);

    return 0;
}


// Обход дерева каталогов
int dopath(Myfunc *func, char *fullpath, int depth)
{
    if (depth < 0)
    {
        chdir(fullpath);
        return 0;
    }

    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
	
	if (lstat(fullpath, &statbuf) == -1)
        return -1;

    if (!S_ISDIR(statbuf.st_mode))
    {
        func(fullpath, &statbuf, FTW_F, depth);
        return 0;
    }

    func(fullpath, &statbuf, FTW_D, depth);

    if ((dp = opendir(fullpath)) == NULL)
        return -1;

    if (chdir(fullpath) == -1)
    {
        closedir(dp);
        return -1;
    }

    depth++;

    // Элемент возврата
    struct stackItem item = {.fileName = "..", .depth = -1};
	if (push(&stack_t, &item) == -1)
	{
        closedir(dp);
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
			if (lstat(dirp->d_name, &statbuf) == -1)
				return -1;
	
			if (!S_ISDIR(statbuf.st_mode))
			{
				func(dirp->d_name, &statbuf, FTW_F, depth);
			}
			else
			{
				strcpy(item.fileName, dirp->d_name);
				item.depth = depth;

				if (push(&stack_t, &item) == -1)
				{
					closedir(dp);
					return -1;
				}
			}
        }
    }

    if (closedir(dp) == -1)
        return -1;

    return 0;
}


static int my_ftw(char *pathname, Myfunc *func)
{
    
    if (chdir(pathname) == -1)
    {
        printf("%sНе удалось выполнить перехол в переданный каталог. %s\n", RED, RESET_COLOR);
        return -1;
    }

    init(&stack_t);

    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        printf("%s Ошибка в функции getcwd %s\n", RED, RESET_COLOR);
        return -1;
    }
	
	struct stackItem item = {.depth = 0};
    strcpy(item.fileName, cwd);
    if (push(&stack_t, &item) == -1)
		return -1;

    while (!is_empty(&stack_t))
    {
        dopath(func, item.fileName, item.depth);
        item = pop(&stack_t);
    }

    printStats();

    return 0;
}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("%sИспользование: %s <начальный_каталог>%s\n", RED, argv[0], RESET_COLOR);
        return 1;
    }

    return my_ftw(argv[1], increment_stats);
}


