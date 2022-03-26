#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "detailed_info.h"

extern char *STAT_DETAILED_INFO[];
extern char *STATM_DETAILED_INFO[];
extern char *IO_DETAILED_INFO[];



#define BUFFSIZE 10000
#define N_LINE 20
#define DETAILED 1
#define CONTENT_SEPARATOR "\n--------------------------------------------------------------------------------------------------\n"

int PID;

void lines()
{
    //printf(CONTENT_SEPARATOR);
    for (int i = 0; i < N_LINE; i++)
        printf("\n");
}

void printCMDLINE()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/cmdline", PID);
    
    FILE *file = fopen(pathToOpen, "r");
    char buf[BUFFSIZE];
    int len = fread(buf, sizeof(char), BUFFSIZE, file);
    buf[len - 1] = 0;
    fclose(file);
    

    printf("Файл cmdline: содержит полную командную строку процесса, если процесс не находится в состоянии зомби, иначе файл пуст.\nСодержимое:");
    printf(CONTENT_SEPARATOR);
    //(через это состояние проходят все процессы, отобраны все ресурсы, кроме последнего – строки в таблице процессов (декскпиптора), чтобы предок смог получить статус завершения потомка.
    printf("%s\n", buf);
    printf(CONTENT_SEPARATOR);
}

void printCWD()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/cwd", PID);

    char buf[BUFFSIZE] = {'\0'};

    readlink(pathToOpen, buf, BUFFSIZE);
    
    printf("Символическая ссылка cwd: указывает на директорию процесса (специальный файл, который сожержит путь к файлу).\nСодержимое:");
    printf(CONTENT_SEPARATOR);
    printf("%s", buf);
    printf(CONTENT_SEPARATOR);
}

void printENVIRON()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/environ", PID);
    FILE *file = fopen(pathToOpen, "r");

    int len;
    char buf[BUFFSIZE];
    printf("Файл environ: список окружения процесса (исходная среда, которая была установлена при запуске выполняемой программы).\n");
    
    if (DETAILED)
    {
        printf(ENVIRON_DETAILED_INFO);
        //Окружение (environment) или среда — это набор пар ПЕРЕМЕННАЯ=ЗНАЧЕНИЕ, доступный каждому пользовательскому процессу. Иными словами, окружение — это набор переменных окружения.
    }
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);
    while ((len = fread(buf, 1, BUFFSIZE, file)) > 0)
    {
        for (int i = 0; i < len; i++)
            if (!buf[i])
                buf[i] = '\n';
        buf[len - 1] = '\n';
        printf("%s", buf);
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
}

void printEXE()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/exe", PID);

    char buf[BUFFSIZE] = {'\0'};

    readlink(pathToOpen, buf, BUFFSIZE);
    
    printf("Символическая ссылка exe: указывает на образ процесса (его exe-файл, фактический путь к выполняемой команде).\n");
    
    if (DETAILED)
    {
        printf(EXE_DETAILED_INFO);
    }
    printf("Содержимое:");
    
    printf(CONTENT_SEPARATOR);
    printf("%s", buf);
    printf(CONTENT_SEPARATOR);
}

void printFD()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/fd/", PID);
    DIR *dir = opendir(pathToOpen);

    printf("Директория fd: cсылки на файлы, которые «открыл» процесс.\n");
    if (DETAILED)
    {
        printf(FD_DETAILED_INFO);
    }
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);
    

    struct dirent *readDir;
    char string[PATH_MAX];
    char path[BUFFSIZE] = {'\0'};
    while ((readDir = readdir(dir)) != NULL)
    {
        if ((strcmp(readDir->d_name, ".") != 0) && (strcmp(readDir->d_name, "..") != 0))
        {
            sprintf(path, "%s%s", pathToOpen, readDir->d_name);
            readlink(path, string, PATH_MAX);
            printf("{%s} -- %s\n", readDir->d_name, string);
        }
    }
    printf(CONTENT_SEPARATOR);
    closedir(dir);
}


void printROOT()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/root", PID);

    char buf[BUFFSIZE] = {'\0'};

    readlink(pathToOpen, buf, BUFFSIZE);
    
    printf("Символическая ссылка root: указывает на корень файловой системы, которой принадлежит процесс.\n");
    
    printf("Содержимое:");
    
    printf(CONTENT_SEPARATOR);
    printf("%s", buf);
    printf(CONTENT_SEPARATOR);
}

void printSTAT()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/stat", PID);
    FILE *file = fopen(pathToOpen, "r");

    char buf[BUFFSIZE];
    fread(buf, 1, BUFFSIZE, file);
    char *token = strtok(buf, " ");

    printf("Файл stat: информация о процессе\n");
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);

    for (int i = 0; token != NULL; i++)
    {
        printf("%-20s", token);
        if (DETAILED)
            printf("    (%s)", STAT_DETAILED_INFO[i]);
        printf("\n");
        
        token = strtok(NULL, " ");
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
}

void printSTATM()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/statm", PID);
    FILE *file = fopen(pathToOpen, "r");

    char buf[BUFFSIZE];
    fread(buf, 1, BUFFSIZE, file);
    char *token = strtok(buf, " ");

    printf("Файл statm: информация об использовании памяти, измеренной в страницах\n");
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);

    for (int i = 0; token != NULL; i++)
    {
        printf("%-20s", token);
        if (DETAILED)
            printf("    (%s)", STATM_DETAILED_INFO[i]);
        printf("\n");
        
        token = strtok(NULL, " ");
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
}



void printMAPS()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/maps", PID);

    char buf[BUFFSIZE] = {'\0'};
    FILE *file = fopen(pathToOpen, "r");
    
    printf("Файл maps: регионы виртуального адресного пространства, выделенные процессу.\n");
    if (DETAILED)
    {
        printf(MAPS_DETAILED_INFO);
    }
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);

    int lengthOfRead;
    while ((lengthOfRead = fread(buf, 1, BUFFSIZE, file)))
    {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
}

void printPAGEMAP()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/pagemap", PID);

    char buf[BUFFSIZE] = {'\0'};
    FILE *file = fopen(pathToOpen, "r");
    
    printf("Файл pagemap: отображение каждой из виртуальных страниц процесса в фреймы физических страниц или область подкачки.\n");
    if (DETAILED)
    {
        printf(PAGEMAP_DETAILED_INFO);
    }
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);

    int len = fread(buf, sizeof(char), BUFFSIZE, file);
    // printf("len = %d\n", len);

    buf[len - 1] = 0;
    printf("%s", buf);
    printf(CONTENT_SEPARATOR);
    fclose(file);
}

void printIO()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/io", PID);
    
    FILE *file = fopen(pathToOpen, "r");
    char buf[BUFFSIZE];
    int len = fread(buf, sizeof(char), BUFFSIZE, file);
    buf[len - 1] = 0;
    fclose(file);
    

    printf("Файл io: cтатистическая информация о работе процесса с устройствами ввода-вывода.\nСодержимое:");
    printf(CONTENT_SEPARATOR);

    char *token = strtok(buf, "\n");

    for (int i = 0; token != NULL; i++)
    {
        printf("%-20s", token);
        if (DETAILED)
            printf("    (%s)", IO_DETAILED_INFO[i]);
        printf("\n");
        
        token = strtok(NULL, "\n");
    }
    printf(CONTENT_SEPARATOR);
}

void choose_pid(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Введите pid процесса для исследования!\n");
        exit(1);
    }
    
    if (strcmp(argv[1], "self") == 0)
        PID = getpid();
    else
        PID = atoi(argv[1]);
    
    char check_dir[PATH_MAX];
    snprintf(check_dir, PATH_MAX, "/proc/%d", PID);

    if (!PID || access(check_dir, F_OK))
    {
        printf("Директория для введенного pid не доступна!\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    choose_pid(argc, argv);
    
    freopen("output.txt", "w", stdout);
    
    printCMDLINE();

    lines();
    printCWD();
    
    lines();
    printEXE();
    
    lines();
    printROOT();
    
    lines();
    printENVIRON();

    
    lines();
    printFD();
    
    lines();
    printIO();

    lines();
    printSTAT();
    
    lines();
    printSTATM();


    lines();
    printMAPS();
    
//    lines();
//    printPAGEMAP();

//smaps

//wchan

    return 0;
}
