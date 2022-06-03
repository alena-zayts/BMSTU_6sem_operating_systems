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
        printf("close() failed\n");
        return;
    }
    if (unlink(SOCK_NAME) == -1) //удаление файла сокета
    {
        printf("unlink() returned -1\n");
    }
}

void sigtstp_handler(int signum)
{
	printf("\nCatch SIGTSTP\n");
    del_socket();
    exit(0);
}

int main(void)
{
    struct sockaddr srvr_name, client_name;
    char buf[BUF_SIZE];
    int  bytes;
	int client_sock_fd, client_namelen;

    if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        printf("socket() failed\n");
        return EXIT_FAILURE;
    }

    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);
	
	//привязывает сокет к локальному адресу
    if (bind(sock_fd, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family)) == -1)
    {
        printf("bind() failed\n");
        del_socket();
        return EXIT_FAILURE;
    }
	
	//размер очереди
	if (listen(sock_fd, 1) == -1)
    {
        printf("listen() failed\n");
        del_socket();
        return EXIT_FAILURE;
    }
    signal(SIGTSTP, sigtstp_handler); //изменение обработчика сигнала (с SIGINT проблемы)
    printf("Listening.\nPress Ctrl + Z to stop...\n");

    while (1)
    {
		//пока не появится "свисток" connect
		client_sock_fd = accept(sock_fd, &client_name, &client_namelen);
		if (client_sock_fd == -1)
		{
			printf("accept() failed\n");
			del_socket();
			return EXIT_FAILURE;
		}
		//может надо адрес из recvfrom взять...
		while (1)
		{
			int b_r = recvfrom(client_sock_fd, buf, sizeof(buf), 0);
			if (b_r == -1) {...}
			...
		}

	
		//блокирует программу до тех пор, пока на входе не появятся новые данные.
		/*
		передается  указатель на еще одну структуру типа sockaddr, в которой функция возвращает данные об адресе клиента, 
		запросившего соединение (в случае файловых сокетов этот параметр не несет полезной информации). 
		Если информация об адресе клиента не нужна, то можно передать значения NULL в предпоследнем и последнем параметрах.
		*/
        bytes = recvfrom(sock_fd, buf, sizeof(buf), 0, NULL, NULL);
        if (bytes < 0)
        {
            del_socket();
            printf("recvfrom() failed\n");
            return EXIT_FAILURE;
        }
        buf[bytes] = 0;
        printf("Server recieved: %s\n", buf);
    }

    del_socket();
    return EXIT_SUCCESS;
}
/*
gcc client.c -o client
gcc server.c -o server

*/
