#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>


#define SOCKET_SERVER_NAME "server_socket"
#define SOCKET_CLIENT_NAME "client_socket_"
#define BUF_SIZE 100


static int sfd;

void cleanup_socket(void) 
{
	close(sfd);
    unlink(SOCKET_CLIENT_NAME);
}

void sighandler(int signum) 
{
    cleanup_socket();
    exit(0);
}

int main(int argc, char* argv[])
{
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) 
	{
        perror("socket failed");
        return errno;
    }

	
	signal(SIGINT, sighandler);
	
	char msg[BUF_SIZE];
    snprintf(msg, BUF_SIZE, "pid_%d", getpid());

    
    /* client socket */
    struct sockaddr_un addr_client;
    addr_client.sun_family = AF_UNIX;
	
    strcpy(addr_client.sun_path, SOCKET_CLIENT_NAME);
    strcat(addr_client.sun_path, msg);
	
    if (bind(sfd, (struct sockaddr*)&addr_client, sizeof(addr_client)) == -1) 
	{
        perror("bind failed");
        cleanup_socket();
        return errno;
	}
    
    /* server socket */
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_SERVER_NAME);
	
	
	/*
    if (connect(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) 
	{
        perror("connect failed");
		cleanup_socket();
        return errno;
    }
	*/
	
    
	size_t bytes;
	
	
	while (1)
	{
		bytes = sendto(sfd, msg, strlen(msg), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
		if (bytes <= 0) 
		{
			printf("sendto failed");
			cleanup_socket();
			return errno;
		}

		/*
		if (send(sfd, msg, strlen(msg), 0) == -1) 
		{ 
			perror("send failed");
			return errno;
		} 
		*/


		char servermsg[BUF_SIZE] = {0};
		bytes = recv(sfd, servermsg, sizeof(servermsg), 0);
		if (bytes < 0) 
		{
			perror("recv failed");
			return errno;
		} 

		printf("Client received message: %s\n", servermsg);
		sleep(3);
	}
	
	
    cleanup_socket();
    return 0;
}
