#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <signal.h>

#define SOCK_PATH "mysocket"
#define BUF_SIZE 100
#define N_CLIENTS 5


int sock_fd;

void del_socket(void)
{
    if (close(sock_fd) == -1) //закрытие сокета
    {
        return;
    }
    if (unlink(SOCK_PATH) == -1) //удаление файла сокета
    {
        return;
    }
}

void sigtstp_handler(int signum)
{
	printf("Catch SIGTSTP\n");
    del_socket();
    exit(0);
}

int main()
{
	int client_sock_fd;
	struct sockaddr_un sock_addr;
	

	signal(SIGTSTP, sigtstp_handler);
	del_socket();
	
	sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock_fd == -1)
	{
		printf("socket() failed\n");
		return EXIT_FAILURE;
	}

	sock_addr.sun_family = AF_UNIX;
	strcpy(sock_addr.sun_path, SOCK_PATH);

	if(bind(sock_fd, (struct sockaddr*)&sock_addr, strlen(sock_addr.sun_path) + sizeof(sock_addr.sun_family)) != 0)
	{
        printf("bind() failed\n");
        del_socket();
        return EXIT_FAILURE;
	}

	if(listen(sock_fd, N_CLIENTS) != 0 )
	{
        printf("listen() failed\n");
        del_socket();
        return EXIT_FAILURE;
	}

	printf("Listening.\nPress Ctrl + Z to stop...\n");	
	
	int bytes;
	char recv_buf[BUF_SIZE];
	char send_buf[BUF_SIZE];
		
	while (1)
	{
		unsigned int sock_len;
		struct sockaddr_un remote_sock_addr;

		if((client_sock_fd = accept(sock_fd, (struct sockaddr*)&remote_sock_addr, &sock_len)) == -1 )
		{
			printf("Error on accept() call \n");
			return 1;
		}


//while (1)

		memset(recv_buf, 0, BUF_SIZE);
		memset(send_buf, 0, BUF_SIZE);
		bytes = recv(client_sock_fd, recv_buf, BUF_SIZE, 0);
		
		if(bytes > 0)
		{
			printf("Server received: %s\n", recv_buf);
			strcpy(send_buf, "Message recieved: ");
			strcat(send_buf, recv_buf);

			if(send(client_sock_fd, send_buf, strlen(send_buf), 0) == -1 )
			{
				printf("send() failed\n");
			}
		}
		else
		{
			printf("recv() failed. Waiting for new connection\n");
			break;
		}

		//close(client_sock_fd);
	}
	
	del_socket();
	return 0;
}
