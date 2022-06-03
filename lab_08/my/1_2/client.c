#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

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

int main(void)
{	//домен соединения (семейство адресов) AF_UNIX: Сокеты в файловом пространстве имен
	//тип SOCK_DGRAM -  определяют ненадежную  службу datagram без установления логического соединения, когда пакеты могут передаваться без сохранения логического порядка
    // протокол - по умолчанию
	//создаёт конечную точку соединения и возвращает файловый дескриптор
	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (sock_fd == -1)
    {
        printf("socket() failed\n");
        return EXIT_FAILURE;
    }
	
	//общая структура адреса (адрес в данном случае -- имя файла)
    struct sockaddr srvr_name;
    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);
	
	char buf[BUF_SIZE];
    sprintf(buf, "pid %d", getpid());
	
	// нужен bind только потому, что клиент заинтересован в получении от сервера сообщения (то есть важен адрес клиента)
	//привязывает сокет к локальному адресу
    if (bind(sock_fd, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family)) == -1)
    {
        printf("bind() failed\n");
        del_socket();
        return EXIT_FAILURE;
    }
	
	// передача данных серверу. 4 параметр 0 - дополнительные флаги, далее адрес сервера и его длина (версия из 1 программы)
	// if (sendto(sock_fd, buf, strlen(buf), 0, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family)) == -1)
	
	// connect (только на стороне клиента). Для установления активного соединения по переданному адресу. 
	// Для протоколов без установленного соединения (UDP например) connect может использоваться, 
	// чтобы указать адрес назначения для всех передаваемых пакетов (адрес сервера)
	if (connect(sock_fd, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family)) == -1)
 	{
		printf("connect() failed\n");
		return EXIT_FAILURE;
 	}
	
	if (sendto(sock_fd, buf, strlen(buf), 0, NULL, NULL) == -1)
    {
        printf("sendto() failed\n");
		del_socket();
        return EXIT_FAILURE;
    }
	
	printf("Client sent: %s\n", buf);
	
    del_socket();
    return EXIT_SUCCESS;
}

//а в предыдущем варианте адрес клиента не играет никакой роли, поэтому можно без bind (привязка сокета к локальному адресу)
