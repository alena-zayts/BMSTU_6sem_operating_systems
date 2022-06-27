#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define SOCK_PATH "mysocket"
#define BUF_SIZE 100


int sock_fd;
void sigtstp_handler(int signum)
{
	printf("Catch SIGTSTP\n");
    close(sock_fd);
    exit(0);
}


int main()
{

	int bytes;
	struct sockaddr_un server_sock_addr;
	char recv_msg[BUF_SIZE];
	char send_msg[BUF_SIZE];

	memset(recv_msg, 0, BUF_SIZE);
	memset(send_msg, 0, BUF_SIZE);
	signal(SIGTSTP, sigtstp_handler);
	
	server_sock_addr.sun_family = AF_UNIX;
	strcpy(server_sock_addr.sun_path, SOCK_PATH);
	memset(send_msg, 0, BUF_SIZE);
	sprintf(send_msg, "pid %d", getpid());
	
	while (1) 
	{
		if((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 )
		{
			printf("socket() failed\n");
			return EXIT_FAILURE;
		}


		if(connect(sock_fd, (struct sockaddr*)&server_sock_addr, strlen(server_sock_addr.sun_path) + sizeof(server_sock_addr.sun_family)) == -1)
		{
			printf("connect() failed %d\n", errno);
			close(sock_fd);
			return EXIT_FAILURE;
		}

	
		if(send(sock_fd, send_msg, strlen(send_msg), 0 ) == -1)
		{
			printf("send() failed\n");
		}
		
		
		memset(recv_msg, 0, BUF_SIZE);

		if((bytes = recv(sock_fd, recv_msg, BUF_SIZE, 0)) > 0 )
		{
			printf("Client received: %s\n", recv_msg);
		}
		else
		{
			if(bytes < 0)
			{
				printf("recv() failed\n");
			}
			else
			{
				printf("Server socket closed \n");
				close(sock_fd);
				break;
			}

		}
		sleep(3);
		close(sock_fd);
	}

	return 0;
}