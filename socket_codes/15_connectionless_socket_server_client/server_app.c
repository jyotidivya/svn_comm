/**
	code for connectionless server
 */

#include "server_header.h"

// #define SERVER_PORT      3555
#define LENGTH_ARRAY     2048
#define DOMAIN			 AF_INET				//for IPv4 internet protocol
#define TYPE			 SOCK_DGRAM				//for connectionless communication
#define PROTOCOL		 0						//protocol value for internet protocol
// #define IP_ADDRESS		 INADDR_ANY				//refers to any host

int sockfd;

int main(int argc, char* argv[])
{
	char dataFromClient[LENGTH_ARRAY];
	char dataToClient[LENGTH_ARRAY];
	char serverIPAddress[30];
	int serverPortNumber;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	int clientAddressLength = sizeof(clientAddress);

	if(argc < 1)
	{
		printf("usage: enter IP Address and server port number\n");
		exit(EXIT_FAILURE);
	}

	strcpy(serverIPAddress, argv[1]);
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

	// assigning the address  specified  by  addr  to  the  socket  
	memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress . sin_family = DOMAIN;
	serverAddress . sin_port = htons(serverPortNumber);
	serverAddress . sin_addr . s_addr = inet_addr(serverIPAddress);

	// binding socket to specific port number and ip address
	if(bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}

	// receiving request from client for the data, recvfrom() function will wait for data
	if(recvfrom(sockfd, dataFromClient, sizeof(dataFromClient), 0, (struct sockaddr *)&clientAddress, &clientAddressLength) == -1)
	{
		perror("recvfrom()!");
		exit(EXIT_FAILURE);
	}

	printf("server received request:%s\n", dataFromClient);
	printf("client port no:%d\nclient address:%s\n", ntohs(clientAddress . sin_port), inet_ntoa(clientAddress . sin_addr));

	while(1)
	{
		memset(dataToClient, '\0', sizeof(dataToClient));
		//send user inputted data to client
		printf("enter data to send\n");
		if(gets(dataToClient) == NULL)
		{
			perror("gets()!");
			exit(EXIT_FAILURE);
		}
		if(sendto(sockfd, dataToClient, sizeof(dataToClient), 0,(struct sockaddr *)&clientAddress, sizeof(clientAddress)) == -1)
		{
			perror("sendto()!");
			exit(EXIT_FAILURE);
		}
		printf("length of send data:%d\n", strlen(dataToClient));

	} 

	close(sockfd);

	return(EXIT_SUCCESS);
}

void handler(int signum)
{
	close(sockfd);
	exit(EXIT_SUCCESS);
}