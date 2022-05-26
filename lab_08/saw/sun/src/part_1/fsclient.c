#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "socket.h"

void CreateMessage(char buf[MAX_MSG_LEN], int argc, char *argv[])
{
	long int curr_time = time(NULL); // Считываем текущее время.

	sprintf(buf, "\n\n" YELLOW "%d" GREEN "\ntime: %ssay: " BLUE, getpid(), ctime(&curr_time));

	if (argc < 2)
		strcat(buf, "Hello world!");
	else
		strcat(buf, argv[1]);
}

// Можно передать сообщение, как аргумент командной строки
// По умолчанию будет передаваться строка "Hello world".
int main(int argc, char *argv[])
{
	struct sockaddr srvr_name; // Данные об адресе сервера.
	int sock;				   // Дескриптор сокета.

	char buf[MAX_MSG_LEN];
	CreateMessage(buf, argc, argv);
	// printf("%s", buf);

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("socket failed");
		return ERROR_CREATE_SOCKET;
	}

	srvr_name.sa_family = AF_UNIX;
	strcpy(srvr_name.sa_data, SOCK_NAME);

	// 0 - доп флаги.
	if (sendto(sock, buf, strlen(buf) + 1, 0, &srvr_name, LEN_STRUCT_SOCKADDR(srvr_name)) < 0)
	{
		perror("sendto failed");
		return ERROR_SENDTO_SOCKET;
	}

	close(sock);
	printf("Send!");
	return OK;
}
