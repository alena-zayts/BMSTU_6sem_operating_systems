#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void *thread_func(void *args)
{
    int flag = 1;
    FILE *fs = (FILE *)args;

    while (flag == 1)
    {
        char c;
        if ((flag = fscanf(fs, "%c", &c)) == 1)
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
        if ((flag = fscanf(fs1, "%c", &c)) == 1)
        {
            fprintf(stdout, "Main thread read: %c\n", c);
        }
    }
	// The pthread_join() function suspends processing of the calling thread until the target thread completes.
	// 2 arg - void **status
    pthread_join(thread, NULL);

    close(fd);

    return 0;
}
