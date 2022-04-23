#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

void *thread_func(void *args)
{
    int fd2 = open("alphabet.txt", O_RDONLY);
    int rc = 1;

    while (rc == 1)
    {
        char c;
        rc = read(fd2, &c, 1);
        if (rc == 1)
        {
            write(1, &c, 1);
        }
    }
    close(fd2);
}

int main(void)
{
    int fd1 = open("alphabet.txt", O_RDONLY);
    

    pthread_t thread;

    if (pthread_create(&thread, NULL, thread_func, 0) != 0)
	{
		perror("error in pthread_create\n");
		return -1;
	}

    int rc = 1;

    while (rc == 1)
    {
        char c;
        rc = read(fd1, &c, 1);
        if (rc == 1)
        {
            write(1, &c, 1);
        }
    }

    pthread_join(thread, NULL);
    close(fd1);

    return 0;
}
