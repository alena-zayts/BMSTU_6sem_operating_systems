#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#define _XOPEN_SOURCE 700
#include <fcntl.h> /* open */
#include <stdint.h> /* uint64_t  */
#include <stdlib.h> /* size_t */
#include <unistd.h> /* pread, sysconf */
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
    //(через это состояние проходят все процессы, отобраны все ресурсы, кроме последнего – строки в таблице процессов (декскриптора), чтобы предок смог получить статус завершения потомка.
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


void printWCHAN()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/wchan", PID);

    char buf[BUFFSIZE] = {'\0'};

    readlink(pathToOpen, buf, BUFFSIZE);
    
    printf("Символическая ссылка wchan: указывает на местоположению в  ядре, в котором процесс спит.\nСодержимое:");
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


void printSMAPS()
{
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/smaps", PID);

    char buf[BUFFSIZE] = {'\0'};
    FILE *file = fopen(pathToOpen, "r");
    
    printf("Файл smaps: потребление памяти для каждого mapping процесса.\n");

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

/*
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
*/

typedef struct {
    uint64_t pfn : 54;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} PagemapEntry;

int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr)
{
    size_t nread;
    ssize_t ret;
    uint64_t data;

    nread = 0;
    while (nread < sizeof(data)) {
        ret = pread(pagemap_fd, ((uint8_t*)&data) + nread, sizeof(data) - nread,
                (vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
        nread += ret;
        if (ret <= 0) {
            return 1;
        }
    }
    entry->pfn = data & (((uint64_t)1 << 55) - 1);
    entry->soft_dirty = (data >> 55) & 1;
    entry->file_page = (data >> 61) & 1;
    entry->swapped = (data >> 62) & 1;
    entry->present = (data >> 63) & 1;
    return 0;
}

int virt_to_phys_user(uintptr_t *paddr, pid_t pid, uintptr_t vaddr)
{
    char pagemap_file[BUFSIZ];
    int pagemap_fd;

    snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%ju/pagemap", (uintmax_t)pid);
    pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0) {
        return 1;
    }
    PagemapEntry entry;
    if (pagemap_get_entry(&entry, pagemap_fd, vaddr)) {
        return 1;
    }
    close(pagemap_fd);
    *paddr = (entry.pfn * sysconf(_SC_PAGE_SIZE)) + (vaddr % sysconf(_SC_PAGE_SIZE));
    return 0;
}

void printPAGEMAP()
{
    char buffer[BUFSIZ];
    char maps_file[BUFSIZ];
    char pagemap_file[BUFSIZ];
    int maps_fd;
    int offset = 0;
    int pagemap_fd;

    snprintf(maps_file, sizeof(maps_file), "/proc/%ju/maps", (uintmax_t)PID);
    snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%ju/pagemap", (uintmax_t)PID);
    maps_fd = open(maps_file, O_RDONLY);
    if (maps_fd < 0) {
        perror("open maps");
    }
    pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0) {
        perror("open pagemap");
    }
	
	printf("Файл pagemap: отображение каждой из виртуальных страниц процесса в фреймы физических страниц или область подкачки.\n");
    printf("addr pfn(page frame number) soft-dirty file/shared swapped present library\n");
	printf("Содержимое:\n");
	printf(CONTENT_SEPARATOR);
	
    for (;;) {
        ssize_t length = read(maps_fd, buffer + offset, sizeof buffer - offset);
        if (length <= 0) break;
        length += offset;
        for (size_t i = offset; i < (size_t)length; i++) {
            uintptr_t low = 0, high = 0;
            if (buffer[i] == '\n' && i) {
                const char *lib_name;
                size_t y;
                {
                    size_t x = i - 1;
                    while (x && buffer[x] != '\n') x--;
                    if (buffer[x] == '\n') x++;
                    while (buffer[x] != '-' && x < sizeof buffer) {
                        char c = buffer[x++];
                        low *= 16;
                        if (c >= '0' && c <= '9') {
                            low += c - '0';
                        } else if (c >= 'a' && c <= 'f') {
                            low += c - 'a' + 10;
                        } else {
                            break;
                        }
                    }
                    while (buffer[x] != '-' && x < sizeof buffer) x++;
                    if (buffer[x] == '-') x++;
                    while (buffer[x] != ' ' && x < sizeof buffer) {
                        char c = buffer[x++];
                        high *= 16;
                        if (c >= '0' && c <= '9') {
                            high += c - '0';
                        } else if (c >= 'a' && c <= 'f') {
                            high += c - 'a' + 10;
                        } else {
                            break;
                        }
                    }
                    lib_name = 0;
                    for (int field = 0; field < 4; field++) {
                        x++;
                        while(buffer[x] != ' ' && x < sizeof buffer) x++;
                    }
                    while (buffer[x] == ' ' && x < sizeof buffer) x++;
                    y = x;
                    while (buffer[y] != '\n' && y < sizeof buffer) y++;
                    buffer[y] = 0;
                    lib_name = buffer + x;
                }
                /* Get info about all pages in this page range with pagemap. */
                {
                    PagemapEntry entry;
                    for (uintptr_t addr = low; addr < high; addr += sysconf(_SC_PAGE_SIZE)) {
                        /* TODO always fails for the last page (vsyscall), why? pread returns 0. */
                        if (!pagemap_get_entry(&entry, pagemap_fd, addr)) {
                            fprintf(stdout, "%jx %jx %u %u %u %u %s\n",
                                (uintmax_t)addr,
                                (uintmax_t)entry.pfn,
                                entry.soft_dirty,
                                entry.file_page,
                                entry.swapped,
                                entry.present,
                                lib_name
                            );
                        }
                    }
                }
                buffer[y] = '\n';
            }
        }
    }
    close(maps_fd);
    close(pagemap_fd);
	printf(CONTENT_SEPARATOR);
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

void print_other_info()
{
    char buf[BUFFSIZE] = {'\0'};
    FILE *file;
	int lengthOfRead;
	
	printf("Общая информация о системе:\n\n\n");
    
	
    printf("Файл /proc/filesystems: Список файловых систем, которые были скомпилированы в ядро или модули ядра которого загружены в данный момент. "
		   "Если файловая система помечена nodev, для нее не требуется монтировать блочное устройство (например, виртуальную файловую систему, сетевую файловую систему).\n");
    printf("Содержимое:\n");
    printf(CONTENT_SEPARATOR);
	
	file = fopen("/proc/filesystems", "r");
    while ((lengthOfRead = fread(buf, 1, BUFFSIZE, file)))
    {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
    

    
    
    
    printf("Директория proc/fs: подкаталоги, которые, в свою очередь, содержат файлы с информацией об (определенных) смонтированных файловых системах.\n");
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);
    DIR *dir = opendir("/proc/fs");
 
    struct dirent *readDir;
    while ((readDir = readdir(dir)) != NULL)
    {
        if ((strcmp(readDir->d_name, ".") != 0) && (strcmp(readDir->d_name, "..") != 0))
        {
            printf("%s\n", readDir->d_name);
        }
    }
    printf(CONTENT_SEPARATOR);
    closedir(dir);
    
    
    
    
    
    printf("Файл /proc/interrupts: количество прерываний на процессор для каждого устройства ввода-вывода, а также прерывания, внутренние для системы (то есть не связанные с устройством как таковым), такие как NMI (немаскируемое прерывание), LOC (прерывание локального таймера), а для систем SMP - TLB (TLB прерывание сброса), RES (прерывание с перепланированием), CAL (прерывание удаленного вызова функции) и, возможно, другие.\n");
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);
    
    file = fopen("/proc/interrupts", "r");
    while ((lengthOfRead = fread(buf, 1, BUFFSIZE, file)))
    {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
    
    
    
    
    
    
    printf("Файл /proc/modules: перечень модулей, которые были загружены системой.\n");
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);
    
    file = fopen("/proc/modules", "r");
    while ((lengthOfRead = fread(buf, 1, BUFFSIZE, file)))
    {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
    
    
    
    
    printf("Файл /proc/ioports: список используемых в настоящее время зарегистрированных областей портов ввода-вывода (порт - адрес или диапазон адресов)\n");
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);
    
    file = fopen("/proc/ioports", "r");
    while ((lengthOfRead = fread(buf, 1, BUFFSIZE, file)))
    {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
    
    
    
    
    printf("Файл /proc/meminfo: статистические данные об использовании памяти в системе. Он используется free(1) для отображения объема свободной и используемой памяти (как физической, так и подкачки) в системе, а также общей памяти и буферов, используемых ядром. Каждая строка файла состоит из имени параметра, за которым следует двоеточие, значения параметра и единицы измерения параметра.\n");
    printf("Содержимое:");
    printf(CONTENT_SEPARATOR);
    
    file = fopen("/proc/meminfo", "r");
    while ((lengthOfRead = fread(buf, 1, BUFFSIZE, file)))
    {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }
    printf(CONTENT_SEPARATOR);
    fclose(file);
	
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
    
    lines();
    printPAGEMAP();
    
    lines();
    printSMAPS();
    
    lines();
    printWCHAN();
	
	lines();
    print_other_info();

    return 0;
}
