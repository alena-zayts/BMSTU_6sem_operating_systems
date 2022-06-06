#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>


#define SOCKET_NAME "mysocket"
#define BUF_SIZE 100


int main(int argc, char* argv[])
{
	int sfd;
	//
    if ((sfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) 
	{
        perror("socket failed");
        return errno;
    }
	close(sfd);
	//

	
	if ((sfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
        perror("socket failed");
        return errno;
    }

    
    /* client socket */
	/*
    struct sockaddr_un addr_client;
    addr_client.sun_family = AF_UNIX;
    
    char full_path_client[BUF_SIZE] = {0};
    strcpy(full_path_client, SOCKET_NAME);
    strcat(full_path_client, CLIENT_PREFIX);
	
    strcpy(addr_client.sun_path, full_path_client);
	*/
    /*
    if (bind(sfd, (struct sockaddr*)&addr_client, sizeof(addr_client)) == -1) 
	{
        perror("bind failed");
        cleanup_socket();
        return errno;
    }*/
    
    /* server socket */
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_NAME);
	
	if (bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
	{
        perror("bind failed\n");
        //cleanup_socket();
        return errno;
    }
	
	
    char send_msg[BUF_SIZE];
    snprintf(send_msg, BUF_SIZE, "pid = %d", getpid());

    if (sendto(sfd, send_msg, strlen(send_msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
	{ 
        perror("send failed");
		close(sfd);
        return errno;
    } 
	
	printf("wait for message\n");

    char recv_msg[BUF_SIZE] = {0};       
    size_t bytes = recvfrom(sfd, recv_msg, sizeof(recv_msg), 0, NULL, NULL);
	printf("recieved\n");
    if (bytes < 0) 
	{
        perror("recv failed");
		close(sfd);
        return errno;
    } 

    printf("Client received message: %s\n", recv_msg);

    close(sfd);
    return 0;
}
