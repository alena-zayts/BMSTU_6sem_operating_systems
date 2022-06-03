#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define BUF_SIZE 256
#define PORT 3425
#define MAX_CLIENTS_NUMB 5


int sock_fd;
//пользуемся recv и send, так как сокет соединен (connect)


void del_socket(void)
{
    if (close(sock_fd) == -1) //закрытие сокета
    {
        printf("close() failed\n");
        return;
    }
}

void sigtstp_handler(int signum)
{
	printf("\nCatch SIGTSTP\n");
    del_socket();
    exit(0);
}



void receive_from_clients(int *clients, fd_set *fd_readset)
{
    char buf[BUF_SIZE];
    int bytes;

    for (int i = 0; i < MAX_CLIENTS_NUMB; i++)
    {
        // FD_ISSET проверяет, является ли описатель частью набора
        if (FD_ISSET(clients[i], fd_readset))
        {
            bytes = recv(clients[i], buf, BUF_SIZE, 0);

            if (bytes <= 0)
            {
                // Соединение разорвано, удаляем сокет из множества
                printf("Client %d disconnected\n", i);
                close(clients[i]);
                clients[i] = 0;
            }
            else
            {
                // Отправляем данные обратно клиенту
                buf[bytes] = 0;
                printf("Server recieved from client %d: %s\n", i, buf);
                
                buf[bytes] = ' ';
                buf[bytes + 1] = 'o';
                buf[bytes + 2] = 'k';
                buf[bytes + 3] = 0;
                send(clients[i], buf, bytes + 3, 0);
            }
        }
    }
}

int main(void)
{
     struct sockaddr_in serv_addr;
     fd_set fd_readset;
     int clients[MAX_CLIENTS_NUMB] = {0};

     sock_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (sock_fd == -1)
     {
       printf("socket() failed\n");
       return EXIT_FAILURE;
     }
	
	/*
	По умолчанию функция socket() создает блокирующий сокет. 
	Чтобы сделать его не- блокирующим, надо использовать функцию fcntl(2) с флагом O_NONBLOCK:
	Теперь любой вызов функции read() для сокета sock_fd будет возвращать управление сразу же. 
	Если на входе сокета нет данных для чтения, функция read() вернет значение EAGAIN. 
	*/
     fcntl(sock_fd, F_SETFL, O_NONBLOCK);

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY; //сервер зарегистрируется на всех адресах той машины, на которой она выполняется.
     serv_addr.sin_port = htons(PORT);

     if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
     {
       printf("bind() failed\n");
	   del_socket();
       return EXIT_FAILURE;
     }

     if (listen(sock_fd, MAX_CLIENTS_NUMB) == -1)
     {
         printf("listen() failed\n");
         del_socket();
         return EXIT_FAILURE;
     }

     signal(SIGTSTP, sigtstp_handler); //изменение обработчика сигнала (с SIGINT проблемы)
     printf("Listening.\nPress Ctrl + Z to stop...\n");

     while(1)
     {
         // initialize the descriptor set to the null set.
         FD_ZERO(&fd_readset);
         // add the file descriptor fd to the set. If the file descriptor fd is already in this set, there shall be no effect on the set, nor will an error be returned.
         FD_SET(sock_fd, &fd_readset);
         int max_fd = sock_fd;

         for (int i = 0; i < MAX_CLIENTS_NUMB; i++)
         {
             if (clients[i])
             {
                 FD_SET(clients[i], &fd_readset);
             }
             if (max_fd < clients[i])
                max_fd = clients[i];
         }
         
         /*
         select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
         n на единицу больше самого большого номера описателей из всех наборов.
         Второй, третий и четвертый параметры - наборы дескрипторов, которые следует проверять, на готовность к 
         чтению, записи и на наличие исключительных ситуаций. Сама функция select() – блокирующая, она возвращает 
         управление, если хотя бы один из проверяемых сокетов готов к выполнению операции. 
         В качестве последнего параметра - интервал времени, по прошествии которого она вернет управление в любом случае.
         возвращают количество описателей, находящихся в наборах, причем это количество может быть равным нулю, 
         если время ожидания на исходе, а интересующие нас события так и не произошли.
         */
         if (select(max_fd + 1, &fd_readset, NULL, NULL, NULL) <= 0)
         {
             printf("select() failed\n");
             del_socket();
             return EXIT_FAILURE;
         }

         // FD_ISSET проверяет, является ли описатель частью набора
         if (FD_ISSET(sock_fd, &fd_readset))
         {
             // Поступил новый запрос на соединение
             // пока не появится "свисток" connect (NULL - адрес запросившего)
             // тут адрес можно сделать
             int new_sock = accept(sock_fd, NULL, NULL);

             if (new_sock == -1)
             {
                 printf("accept() failed\n");
                 del_socket();
                 return EXIT_FAILURE;
             }

             fcntl(new_sock, F_SETFL, O_NONBLOCK);

             int not_found = 1;
             for (int i = 0; i < MAX_CLIENTS_NUMB && not_found; i++)
             {
                 if (!clients[i])
                 {
                     clients[i] = new_sock;
                     printf("New connection %d\n", i);
                     not_found = 0;
                 }
             }
         }

         receive_from_clients(clients, &fd_readset);
     }
     return EXIT_SUCCESS;
}
