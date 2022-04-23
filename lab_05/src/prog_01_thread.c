#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#define OK 0
#define BUF_SIZE 20
#define VALID_READED 1

#define FILE_NAME "data/alphabet.txt"
#define SPEC "%c\n"

void *run_buffer(void *args)
{
    // fprintf(stdout, "\n============ IN RUN_BUFFER ==============");
    int flag = 1;
    FILE *fs = (FILE *)args;

    while (flag == VALID_READED)
    {
        char c;
        if ((flag = fscanf(fs, SPEC, &c)) == VALID_READED)
        {
            // fprintf(stdout, "\nPRINT IN RUN_BUFFER: ");
            fprintf(stdout, "thread 2: " SPEC, c);
        }
    }

    // fprintf(stdout, "\n======== LEAVE RUN_BUFFER ==========");

}

int main(void)
{
    setbuf(stdout, NULL);

    pthread_t thread;
    int fd = open(FILE_NAME, O_RDONLY);

    FILE *fs1 = fdopen(fd, "r");
    char buff1[BUF_SIZE];
    setvbuf(fs1, buff1, _IOFBF, BUF_SIZE);

    FILE *fs2 = fdopen(fd, "r");
    char buff2[BUF_SIZE];
    setvbuf(fs2, buff2, _IOFBF, BUF_SIZE);

    int rc = pthread_create(&thread, NULL, run_buffer, (void *)fs2);

    int flag = 1;
    while (flag == VALID_READED)
    {
        char c;
        // fprintf(stdout, "\nSCANF IN MAIN_1");
        flag = fscanf(fs1, SPEC, &c);
        // fprintf(stdout, "\nSCANF IN MAIN_2");
        if (flag == VALID_READED)
        {
            // fprintf(stdout, "\nPRINT IN MAIN: ");
            fprintf(stdout, "thread 1: " SPEC, c);
        }
    }

    pthread_join(thread, NULL);

    return OK;
}

/*
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

void *thread_func(void *args)
{
    int flag = 1;
    FILE *fs = (FILE *)args;

    while (flag == 1)
    {
        char c;
        if ((flag = fscanf(fs, "%c\n", &c)) == 1)
        {
            fprintf(stdout, "Additional thread read: %c\n", c);
        }
    }
}

int main(void)
{
    setbuf(stdout, NULL);

    pthread_t thread;
    int fd = open("alphabet.txt", O_RDONLY);

    FILE *fs1 = fdopen(fd, "r");
    char buff1[20];
    setvbuf(fs1, buff1, _IOFBF, 20);

    FILE *fs2 = fdopen(fd, "r");
    char buff2[20];
    setvbuf(fs2, buff2, _IOFBF, 20);

    if (pthread_create(&thread, NULL, thread_func, (void *)fs2) != 0)
	{
		perror("Error in pthread_create\n");
		return -1;
	}

    int flag = 1;
    while (flag == 1)
    {
        char c;
        if ((flag = fscanf(fs1, "%c\n", &c)) == 1)
        {
            fprintf(stdout, "Main thread read: %c\n", c);
        }
    }
	// The pthread_join() function suspends processing of the calling thread until the target thread completes.
	// 2 arg - void **status
    pthread_join(thread, NULL);
    //wait(NULL);

    return 0;
}
*/