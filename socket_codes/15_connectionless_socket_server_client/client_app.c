/**
	code for connectionless client
 */

#include "client_header.h"

// #define SERVER_PORT      3555
#define LENGTH_ARRAY     1024
#define DOMAIN			 AF_INET			//for IPv4 internet protocol
#define TYPE			 SOCK_DGRAM			//for connectionless communication
#define PROTOCOL		 0					//protocol value for internet protocol

int sockfd;

int main(int argc, char *argv[])
{
	int serverPortNumber;
	char serverIPAddress[30];
	char dataToServer[LENGTH_ARRAY] = "send data";
	char dataFromServer[LENGTH_ARRAY];
	struct sockaddr_in serverAddress;
	int serverAddressLength = sizeof(serverAddress);
	if(argc < 1)
	{
		printf("usage: enter IP Address and port number\n");
		exit(EXIT_FAILURE);
	}

	strcpy(serverIPAddress, argv[1]);
	puts(serverIPAddress);
	serverPortNumber = atoi(argv[2]);

	// catching SIGINT and handling through handler
	if(signal(SIGINT, handler) == SIG_ERR)
	{
		perror("signal()!");
		exit(EXIT_FAILURE);
	}

	//creating the socket
	if((sockfd = socket(DOMAIN, TYPE, PROTOCOL)) == -1)
	{
		perror("socket()!");
		exit(EXIT_FAILURE);
	}

	memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress . sin_family = DOMAIN;
	serverAddress . sin_port = htons(serverPortNumber);
	serverAddress . sin_addr . s_addr = inet_addr(serverIPAddress);
	printf("Address in binary:%x\n", serverAddress . sin_addr . s_addr);

	// send request to server to send data

	if(sendto(sockfd, dataToServer, sizeof(dataToServer), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress) ) == -1)
	{
		perror("sendto()!");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		// receive data from the server

		if(recvfrom(sockfd, dataFromServer, sizeof(dataFromServer), MSG_TRUNC, (struct sockaddr *)&serverAddress, &serverAddressLength) == -1)
		{
			perror("recvfrom()!");
			exit(EXIT_FAILURE);
		}
		int length = strlen(dataFromServer);
		printf("length of received data:%d\n", length);
		puts(dataFromServer);
		

	}
}

void handler(int signum)
{
	close(sockfd);
	exit(EXIT_SUCCESS);
}