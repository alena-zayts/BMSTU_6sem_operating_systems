#include <dirent.h> 
#include <limits.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

#define FTW_F 1 
#define FTW_D 2 
#define FTW_DNR 3 
#define FTW_NS 4 

#define RESET "\033[0m"
#define YELLOW "\033[0;33m"
#define PURPLE "\033[0;35m"
#define BLUE "\033[0;36m"

typedef int Myfunc(const char *, const struct stat *, int, int);

static Myfunc increment_stats;
static int my_ftw(char *, Myfunc * );
static int dopath(const char* filename, int depth, Myfunc * );

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;


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
    printf(RESET);
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

    printf("    ├── %s%s\n", pathname, RESET);

    return 0;
}

static int dopath(const char* filename, int depth, Myfunc * func)
{
    struct stat statbuf;
    struct dirent * dirp;
    DIR * dp;
    int ret = 0;

    if (lstat(filename, &statbuf) < 0) 
        return(func(filename, &statbuf, FTW_NS, depth));

    if (!S_ISDIR(statbuf.st_mode))
        return(func(filename, &statbuf, FTW_F, depth));
	

    if ((ret = func(filename, &statbuf, FTW_D, depth)) != 0)
        return(ret);

    if ((dp = opendir(filename)) == NULL)
        return(func(filename, &statbuf, FTW_DNR, depth));
    
    chdir(filename);

    while ((dirp = readdir(dp)) != NULL && ret == 0)
    {
        if (strcmp(dirp->d_name, ".") != 0 &&
            strcmp(dirp->d_name, "..") != 0 )
        {
            ret = dopath(dirp->d_name, depth+1, func);
        }
    }
    
    chdir(".."); 

    if (closedir(dp) < 0)
        perror("Невозможно закрыть каталог");

    return(ret);    
}

static int my_ftw(char * pathname, Myfunc * func)
{
    return(dopath(pathname, 0, func));
} 


int main(int argc, char * argv[])
{
   int ret; 
   if (argc != 2)
   {
      printf("Использование: ftw <начальный каталог>\n");
      exit(-1);
   }

	ret = my_ftw(argv[1], increment_stats); 
	if (ret == 0)
		printStats();

    exit(ret);
}



