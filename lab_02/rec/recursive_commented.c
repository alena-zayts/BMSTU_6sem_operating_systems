#include <dirent.h> 
#include <limits.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

#define FTW_F 1 //файл, не являющийся каталогом
#define FTW_D 2 //каталог
#define FTW_DNR 3 //каталог, недоступный для чтения
#define FTW_NS 4 //файл, информацию о котором нельзя получить с помощью stat

#define RESET "\033[0m"
#define CYAN "\033[0;36m"
#define PURPLE "\033[0;35m"
#define YELLOW "\033[0;33m"

// тип функции, которая будет вызываться для каждого встреченного файла
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

//обход дерева каталогов, начиная с fullpath
static int dopath(const char* filename, int depth, Myfunc * func)
{
    struct stat statbuf;
    struct dirent * dirp;
    DIR * dp;
    int ret = 0;

//stat возвращает информацию о файле file_name и заполняет буфер buf. 
//lstat идентична stat, но в случае символьных сылок она возвращает информацию о самой ссылке, 
//а не о файле, на который она указывает. 
    if (lstat(filename, &statbuf) < 0) //ошибка вызова функции lstat   
        return(func(filename, &statbuf, FTW_NS, depth));

    if (!S_ISDIR(statbuf.st_mode)) //не каталог 
        return(func(filename, &statbuf, FTW_F, depth)); //отобразить в дереве 
		
	/*
 * Это каталог. Сначала вызвать функцию func(), 
 * а затем обработать все файлы в этом каталоге. 
 */

    if ((ret = func(filename, &statbuf, FTW_D, depth)) != 0)
        return(ret);

    if ((dp = opendir(filename)) == NULL) //каталог недоступен
        return(func(filename, &statbuf, FTW_DNR, depth));
    
    chdir(filename);
	//Функция readdir() возвращает указатель на следующую запись каталога в структуре dirent, 
	//прочитанную из потока каталога. Каталог указан в dir. 
	//Функция возвращает NULL по достижении последней записи или если была обнаружена ошибка.
    while ((dirp = readdir(dp)) != NULL && ret == 0)
    {
        if (strcmp(dirp->d_name, ".") != 0 &&
            strcmp(dirp->d_name, "..") != 0 ) //пропустить каталоги . и ..
        {
            ret = dopath(dirp->d_name, depth+1, func); /* рекурсия */
        }
    }
    
	//чтобы каждый раз, когда встречается каталог, функции lstat() передавался не полный путь к файлу, 
	//а только его имя. 
    chdir(".."); 

    if (closedir(dp) < 0)
        perror("Невозможно закрыть каталог");

    return(ret);    
}

//обоходит дерево каталогов, начиная с pathname и применяя к каждому файлу func
/*
ftw() перемещается по дереву каталогов, начиная с указанного каталога dir. Для каждого найденного элемента дерева вызываются: 
fn() с указанием полного имени этого элемента, указатель на структуру элемента stat(2) и целое число. 
Значения этого целого числа flag могут быть одним из следующих:
FTW_F
(обычный файл);
FTW_D
(каталог);
FTW_DNR
(каталог, который не может быть считан);
FTW_SL
(символьная ссылка);
FTW_NS
(ошибка в элементе, который не является символьной ссылкой).
	*/
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

	ret = my_ftw(argv[1], increment_stats); //выполняет всю работу
	printStats();

    exit(ret);
}



