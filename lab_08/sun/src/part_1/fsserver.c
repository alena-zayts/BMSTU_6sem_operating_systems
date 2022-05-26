#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "socket.h"

void Cleanup(int sock)
{
	close(sock);
	unlink(SOCK_NAME);
}

int main(int argc, char *argv[])
{
	struct sockaddr srvr_name; // Данные об адресе сервера.
	struct sockaddr rcvr_name; // Данные об адресе клиента, запросившего соединение
	int namelen;			   // Длина возвращаемой структуры с адресом.

	char buf[MAX_MSG_LEN]; // Буфер, в который записываются сообщения от клиентов.
	int sock;			   // Дескриптор сокета.
	int bytes;			   // Кол-во полученных байт.

	long int curr_time = time(NULL); // Считываем текущее время.

	// Создаем сокет. Получаем дескриптор сокета.
	// AF_UNIX - сокеты в файловом пространстве имен.
	// SOCK_DGRAM - датаграмный сокет. Хранит границы сообщений.
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("socket failed");
		return ERROR_CREATE_SOCKET;
	}

	srvr_name.sa_family = AF_UNIX;
	strcpy(srvr_name.sa_data, SOCK_NAME); // sa_data - имя файла сокета.

	// bind() - связывает сокет с заданным адресом.
	// После вызова bind() программа-сервер становится доступна для соединения по заданному адресу (имени файла)
	if (bind(sock, &srvr_name, LEN_STRUCT_SOCKADDR(srvr_name)) < 0)
	{
		perror("bind failed");
		return ERROR_BIND_SOCKET;
	}

	while (TRUE)
	{
		// recvfrom блокирует программу до тех пор, пока на входе не появятся новые данные.
		// bytes = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL); // В нашем случае можно и вот так.
		bytes = recvfrom(sock, buf, sizeof(buf), 0, &rcvr_name, &namelen);

		if (bytes < 0)
		{
			perror("recvfrom failed");
			Cleanup(sock);
			return ERROR_RECVFROM_SOCKET;
		}

		printf("\n\nReceived message: %s" WHITE "\nLen = %ld\n______", buf, strlen(buf));
	}

	Cleanup(sock);

	return OK;
}

// void CheckError(int check, int err, char err_msg[MAX_LEN_ERR_MSG])
// {
// 	if (check < 0)
// 	{
// 		perror(err_msg);
// 		exit(err);
// 	}
// }