#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "stack.h"

#define FTW_F 1 //файл, не являющийся каталогом
#define FTW_D 2 //каталог
#define FTW_DNR 3 //каталог, недоступный для чтения
#define FTW_NS 4 //файл, информацию о котором нельзя получить с помощью stat

#define RESET "\033[0m"
#define RED "\033[0;31m"
#define CYAN "\033[0;36m"
#define PURPLE "\033[0;35m"
#define YELLOW "\033[0;33m"

typedef int Myfunc(const char *, const struct stat *, int, int);

static Myfunc increment_stats;
static int my_ftw(char *, Myfunc * );
int dopath(Myfunc *func, char *fullpath, int depth);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;
static struct stack stk;

void printStats()
{
   ntot = nreg + ndir +  nblk + nchr +  nfifo + nslink + nsock;

    if (ntot == 0)
		ntot = 1;

    printf(YELLOW);
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
            printf(CYAN);
            switch(statptr->st_mode & S_IFMT) /* режим доступа & S_IFMT	0170000	битовая маска для полей типа файла */
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


// Обход дерева каталогов
int dopath(Myfunc *func, char *fullpath, int depth)
{
    if (depth < 0) // Возврат из просмотренного каталога
    {
		//chdir изменяет текущий каталог каталог на path.
        chdir(fullpath);
        return 0;
    }

    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
	
	// stat возвращает информацию о файле file_name и заполняет буфер buf. 
	// lstat идентична stat, но в случае символьных сылок она возвращает информацию о самой ссылке, а не о файле, на который она указывает.
    if (lstat(fullpath, &statbuf) == -1)
        return -1;

    if (!S_ISDIR(statbuf.st_mode))
    {
        // Это не каталог
        func(fullpath, &statbuf, FTW_F, depth);

        return 0;
    }

    // Это каталог
    func(fullpath, &statbuf, FTW_D, depth);

    if ((dp = opendir(fullpath)) == NULL) /* Каталог не доступен */
        return -1;

    if (chdir(fullpath) == -1)
    {
        closedir(dp);
        return -1;
    }

    depth++;

    // Элемент возврата
    struct stackItem item = {.fileName = "..", .depth = -1};
    push(&stk, &item);

    while ((dirp = readdir(dp)) != NULL)
    {
        // Пропуск . и ..
        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
            strcpy(item.fileName, dirp->d_name);
            item.depth = depth;
            push(&stk, &item);
        }
    }

    if (closedir(dp) == -1)
        return -1;

    return 0;
}

// Первичный вызов для переданного программе каталога
static int my_ftw(char *pathname, Myfunc *func)
{
    // Меняем текущую директорию на переданную
    if (chdir(pathname) == -1)
    {
        printf("%sПереданного каталога не существует, либо он не доступен%s", RED, RESET);
        return -1;
    }

    init(&stk);

    struct stackItem item = {.depth = 0};
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        printf("%s Ошибка: невозможно получить путь рабочего каталога %s", RED, RESET);
        return -1;
    }

    strcpy(item.fileName, cwd);
    push(&stk, &item);

    while (!empty(&stk))
    {
        dopath(func, item.fileName, item.depth);
        item = pop(&stk);
    }

    printStats();

    return 0;
}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("%sИспользование: %s <начальный_каталог>%s\n", RED, argv[0], RESET);
        return 1;
    }

    return my_ftw(argv[1], increment_stats);
}


