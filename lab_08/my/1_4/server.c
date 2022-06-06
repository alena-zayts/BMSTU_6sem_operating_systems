#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define SOCKET_NAME "mysocket"
#define BUF_SIZE 100


static int sfd;

void cleanup_socket(void) 
{
    close(sfd);
    unlink(SOCKET_NAME);
}

void sighandler(int signum) 
{
    cleanup_socket();
    exit(0);
}

int main(void) 
{
	//
	if ((sfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) 
	{
        perror("socket failed\n");
        return errno;
    }
	cleanup_socket();
	//
	
    if ((sfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) 
	{
        perror("socket failed\n");
        return errno;
    }

    struct sockaddr_un sock_addr;
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, SOCKET_NAME);
	sleep(10);
    if (bind(sfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) 
	{
        perror("bind failed\n");
        cleanup_socket();
        return errno;
    }

    signal(SIGINT, sighandler);


    char msg_rec[BUF_SIZE];
    char msg_send[BUF_SIZE];
	
    while (1) 
	{
        printf("Waiting for client..\n");
		
        size_t bytes = recvfrom(sfd, msg_rec, BUF_SIZE, 0, NULL, NULL);
        if (bytes == -1) 
		{
            perror("recvfrom failed");
            cleanup_socket();
            return errno;
        }
		
		msg_rec[bytes] = '\0';
		printf("Server received: %s\n", msg_rec);
		

        

        strcpy(msg_send, "Message was recieved: ");
        strcat(msg_send, msg_rec);
        bytes = sendto(sfd, msg_send, strlen(msg_send), 0, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
        if (bytes <= 0) 
		{
            printf("sendto failed");
            cleanup_socket();
            return errno;
        }
		sleep(5);
    }
}
