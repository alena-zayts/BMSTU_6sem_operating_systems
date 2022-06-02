#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#define SOCK_NAME "socket.soc"
#define BUF_SIZE 256

int sock_fd;

void del_socket(void)
{
    if (close(sock_fd) == -1) //закрытие сокета
    {
        printf("close() failed");
        return;
    }
    if (unlink(SOCK_NAME) == -1) //удаление файла сокета
    {
        printf("close() failed");
        return;
    }
}

void sigint_handler(int signum)
{
    del_socket();
    exit(1);
}

int main(void)
{
    struct sockaddr srvr_name, rcvr_name;
    char buf[BUF_SIZE];
    int  namelen, bytes;

    if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() failed");
        return EXIT_FAILURE;
    }

    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);
	
	//привязывает сокет к локальному адресу
    if (bind(sock_fd, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family)) == -1)
    {
        printf("bind() failed");
        del_socket();
        return EXIT_FAILURE;
    }

    signal(SIGINT, sigint_handler); //изменение обработчика сигнала
    printf("Listening.\nPress Ctrl + Z to stop...\n");

    while (1)
    {
        bytes = recvfrom(sock_fd, buf, sizeof(buf),  0, &rcvr_name, &namelen);
        if (bytes < 0)
        {
            del_socket();
            printf("recvfrom() failed");
            return EXIT_FAILURE;
        }
        buf[bytes] = 0;
        rcvr_name.sa_data[namelen] = 0;
        printf("Client (with address %s) sent: %s namelen=%d\n", (char *)rcvr_name.sa_data, buf, namelen);
    }

    del_socket();
    return EXIT_SUCCESS;
}
/*
gcc client.c -o client
gcc server.c -o server

*/