#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

const char *TEMPLATE[] = {
    "pid  '%s'\n",
    "comm '%s'\n",
    "state  '%s'\n",
    "ppid  '%s'\n",
    "pgrp  '%s'\n",
    "session  '%s'\n",
    "tty_nr  '%s'\n",
    "tpgid  '%s'\n",
    "flags  '%s'\n",
    "minflt  '%s'\n",
    "cminflt  '%s'\n",
    "majflt  '%s'\n",
    "cmajflt  '%s'\n",
    "utime  '%s'\n",
    "stime  '%s'\n",
    "cutime  '%s'\n",
    "cstime  '%s'\n",
    "priority  '%s'\n",
    "nice  '%s'\n",
    "num_threads  '%s'\n",
    "itrealvalue  '%s'\n",
    "starttime  '%s'\n",
    "vsize  '%s'\n",
    "rss  '%s'\n",
    "rsslim  '%s'\n",
    "startcode  '%s'\n",
    "endcode  '%s'\n",
    "startstack  '%s'\n",
    "kstkesp  '%s'\n",
    "kstkeip  '%s'\n",
    "signal  '%s'\n",
    "blocked  '%s'\n",
    "sigignore  '%s'\n",
    "sigcatch  '%s'\n",
    "wchan  '%s'\n",
    "nswap  '%s'\n",
    "cnswap  '%s'\n",
    "exit_signal  '%s'\n",
    "processor  '%s'\n",
    "rt_priority  '%s'\n",
    "policy  '%s'\n",
    "delayacct_blkio_ticks  '%s'\n",
    "guest_time  '%s'\n",
    "cguest_time  '%s'\n",
    "start_data  '%s'\n",
    "end_data  '%s'\n",
    "start_brk  '%s'\n",
    "arg_start  '%s'\n",
    "arg_end  '%s'\n",
    "env_start  '%s'\n",
    "env_end  '%s'\n",
    "exit_code  '%s'\n"};

const char *TEMPLATE_STATM[] = {
    "size  '%s'\n",
    "resident '%s'\n",
    "shared  '%s'\n",
    "text  '%s'\n",
    "lib  '%s'\n",
    "data  '%s'\n",
    "dt  '%s'\n"};

void print_cmdline(const int pid)
{
    char path[PATH_MAX];
    char buf[10000];
    snprintf(path, PATH_MAX, "/proc/%d/cmdline", pid);

    FILE *file = fopen(path, "r");
    int len = fread(buf, 1, 10000, file);
    buf[len - 1] = 0;

    printf("\nCMDLINE:\n%s\n", buf);

    fclose(file);
}

void print_environ(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/environ", pid);
    int len;
    char buf[10000];

    FILE *file = fopen(path_to_open, "r");

    printf("\nENVIRON:\n");
    while ((len = fread(buf, 1, 10000, file)) > 0)
    {
        for (int i = 0; i < len; i++)
        {
            if (!buf[i])
            {
                buf[i] = '\n';
            }
        }
        buf[len - 1] = '\n';

        printf("%s", buf);
    }

    fclose(file);
}

void print_fd(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/fd/", pid);
    DIR *dir = opendir(path_to_open);
    struct dirent *readDir;
    char string[PATH_MAX];
    char path[10000] = {'\0'};

    printf("\nFD:\n");
    while ((readDir = readdir(dir)) != NULL)
    {
        if ((strcmp(readDir->d_name, ".") != 0) && (strcmp(readDir->d_name, "..") != 0))
        {
            sprintf(path, "%s%s", path_to_open, readDir->d_name);
            int _read_len = readlink(path, string, PATH_MAX);

            printf("%s -> %s\n", readDir->d_name, string);
        }
    }

    closedir(dir);
}

void print_stat(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/stat", pid);
    char buf[10000];
    FILE *file = fopen(path_to_open, "r");

    fread(buf, 1, 10000, file);
    char *token = strtok(buf, " ");

    printf("\nSTAT: \n");
    for (int i = 0; token != NULL; i++)
    {
        printf(TEMPLATE[i], token);
        token = strtok(NULL, " ");
    }

    fclose(file);
}

void print_statm(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/statm", pid);
    FILE *file = fopen(path_to_open, "r");
    char buf[10000];

    fread(buf, 1, 10000, file);
    char *token = strtok(buf, " ");

    printf("\nSTATM: \n");
    for (int i = 0; token != NULL; i++)
    {
        printf(TEMPLATE_STATM[i], token);
        token = strtok(NULL, " ");
    }

    fclose(file);
}

void print_cwd(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/cwd", pid);
    char buf[10000] = {'\0'};
    int _read_len = readlink(path_to_open, buf, 10000);

    printf("\nCWD:\n%s\n", buf);
}

void print_exe(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/exe", pid);
    char buf[10000] = {'\0'};
    int _read_len = readlink(path_to_open, buf, 10000);

    printf("\nEXE:\n%s\n", buf);
}

void print_maps(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/maps", pid);
    char buf[10000] = {'\0'};
    FILE *file = fopen(path_to_open, "r");
    int lengthOfRead;

    printf("\nMAPS:\n");
    while ((lengthOfRead = fread(buf, 1, 10000, file)))
    {
        buf[lengthOfRead] = '\0';
        printf("%s\n", buf);
    }

    fclose(file);
}

void print_root(const int pid)
{
    char path_to_open[PATH_MAX];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/root", pid);
    char buf[10000] = {'\0'};
    int _read_len = readlink(path_to_open, buf, 10000);

    printf("\nROOT:\n%s\n", buf);
}

int get_pid(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Введите pid процесса для исследования!");
        exit(1);
    }

    int pid = atoi(argv[1]);
    char check_dir[PATH_MAX];
    snprintf(check_dir, PATH_MAX, "/proc/%d", pid);

    if (!pid || access(check_dir, F_OK))
    {
        printf("Директория для введенного pid не доступна!");
        exit(1);
    }

    return pid;
}

int main(int argc, char *argv[])
{
    int pid = get_pid(argc, argv);

    print_cmdline(pid);
    print_environ(pid);
    print_fd(pid);
    print_stat(pid);
    print_statm(pid);
    print_cwd(pid);
    print_exe(pid);
    print_maps(pid);
    print_root(pid);

    return 0;
}
