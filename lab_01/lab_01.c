#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include "sys/file.h"
#include <pthread.h>
#include "signal.h"

#define LOCKFILE "/var/run/my_daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

sigset_t mask;


int lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;			
	return (fcntl(fd, F_SETLK, &fl));
}

int alreadyRunning(void) 
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
    if (fd < 0) 
	{
        syslog(LOG_ERR, "не возможно открыть %s: %s", LOCKFILE,
        strerror(errno));
        exit(1);
    }

	if (lockfile(fd) < 0)
	{
		if (errno == EACCES || errno == EAGAIN)
		{
			close(fd);
			return (1);
		}
		syslog(LOG_ERR, "невозможно установить блокировку на %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}

    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    return 0;
}

void quit(const char *errMessage, const char *command) 
{
    syslog(LOG_ERR, "%s %s", errMessage, command);
    exit(1);
}

void *threadFun(void *arg) 
{
    int err, signo;

    for (;;) 
	{
        syslog(LOG_INFO, "Поток обработки сигналов: %ld.\n", pthread_self());
        err = sigwait(&mask, &signo);
        if (err != 0)
            quit("ошибка вызова функции sigwait\n", "");
        switch (signo) {
            case SIGHUP:
                syslog(LOG_INFO, "получен сигнал SIGHUP; getlogin=%s\n", getlogin());
                break;
            case SIGTERM:
                syslog(LOG_INFO, "получен сигнал SIGTERM; выход\n");
                exit(0);
            default:
                syslog(LOG_INFO, "получен непредвиденный сигнал %d\n", signo);
        }
    }
    return 0;
}

void daemonize(const char *cmd) 
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    
    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        quit("%s: невозможно получить максимальный номер дескриптора\n", cmd);

    if ((pid = fork()) < 0)
        quit("%s: ошибка вызова функции fork\n", cmd);
    else if (pid != 0) 
        exit(0);
    
    setsid();

    // Убрать возможность обретения управляющего терминала
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
	
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        quit("%s: невозможно игнорировать сигнал SIGHUP\n", cmd);

    if (chdir("/") < 0)
        quit("%s: невозможно сделать текущим корневой каталог\n", cmd);

    if (rl.rlim_max == RLIM_INFINITY) 
		rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++) 
		close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    // Инициализировать файл журнала
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) 
	{
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d\n", fd0, fd1, fd2);
        exit(1);
    }
}

int main() {
    daemonize("my_daemon");

    if (alreadyRunning())
		quit("демон уже запущен\n", "");

    int err;
	pthread_t tid;
    struct sigaction sa;

    /*
     * Восстановить действие по умолчанию для сигнала SIGHUP
     * и заблокировать все сигналы
    */
	
    sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
        quit("невозможно восстановить действие SIG_DFL для SIGHUP\n", "");

	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
        quit("ошибка выполнения операции SIG_BLOCK\n", "");

    err = pthread_create(&tid, NULL, threadFun, 0);
	if (err != 0)
        quit("невозможно создать поток обработки сигналов\n", "");


	long int ttime;

	

	while (1)
	{
		ttime = time(NULL);
		syslog(LOG_INFO, "Демон! Время: %s. Идентификатор созданного потока для обработки сигнала: %ld", 
			   ctime(&ttime), tid);
		sleep(3);
	}

}
