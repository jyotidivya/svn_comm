/**
	code for connectionless client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

void handler(int);

// #define SERVER_PORT      3555
#define LENGTH_ARRAY     2048
#define DOMAIN			 AF_INET			//for IPv4 internet protocol
#define TYPE			 SOCK_DGRAM			//for connectionless communication
#define PROTOCOL		 0					//protocol value for internet protocol

#define FILE_NAME 			 	 "test.h264"
#define FILE_OPEN_FLAGS			 O_RDWR | O_APPEND |O_CREAT 
#define FILE_MODE				 S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP  		
#define SIZE_TO_WRITE_TO_FILE	 256
#define ACKNOWLEDGMENT 			 "ACD0CF459ED23B10"

int clientFd,fd;
extern int count;

int main(int argc, char *argv[])
{
	int serverPortNumber, clientPortNumber, numberOfBytesWritten;
	char serverIPAddress[30];
	char clientIPAddress[30];
	char dataToServer[LENGTH_ARRAY] ;
	char dataFromServer[LENGTH_ARRAY];
	char hexString[] = ACKNOWLEDGMENT;
	char *pos = hexString;
	unsigned char ack[9];
	struct sockaddr_in serverAddress, clientAddress;
	int serverAddressLength = sizeof(serverAddress);
	int bytesReceived, i;

	if(argc < 5)
	{
		printf("usage: enter <server IP Address><server port number><client IP address><client port number>\n");
		exit(EXIT_FAILURE);
	}

	strcpy(serverIPAddress, argv[1]);
	serverPortNumber = atoi(argv[2]);
	strcpy(clientIPAddress, argv[3]);
	clientPortNumber = atoi(argv[4]);

	// catching SIGINT and handling through handler
	if(signal(SIGINT, handler) == SIG_ERR)
	{
		perror("signal()!");
		exit(EXIT_FAILURE);
	}

	//creating the socket
	if((clientFd = socket(DOMAIN, TYPE, PROTOCOL)) == -1)
	{
		perror("socket()!");
		exit(EXIT_FAILURE);
	}

	memset((struct sockaddr_in *)&clientAddress, 0, sizeof(clientAddress));
	clientAddress . sin_family = DOMAIN;
	if(inet_aton(clientIPAddress, (struct in_addr *)&(clientAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}
	clientAddress . sin_port = htons(clientPortNumber);

	serverAddress . sin_family = DOMAIN;
	serverAddress . sin_port = htons(serverPortNumber);
	if(inet_aton(serverIPAddress, (struct in_addr *)&(serverAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}

	// binding client socket to particular port and address
	if(bind(clientFd, (struct sockaddr *)&clientAddress, sizeof(clientAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}

	// open the file
	fd = open(FILE_NAME, FILE_OPEN_FLAGS, FILE_MODE);
	if(fd == -1)
	{
		perror("open()failed!");
		exit(EXIT_FAILURE);
	}

	for(i=0;i<9;i++)
	{
		sscanf(pos, "%2hhx", &ack[i]);
		pos += 2;
	}

	do
	{
		
		memset(dataFromServer, '\0', sizeof(dataFromServer));
		// receive data from the server
		if((bytesReceived = recvfrom(clientFd, dataFromServer, sizeof(dataFromServer), 0, NULL, NULL)) == -1)
		{
			perror("recvfrom()!");
			exit(EXIT_FAILURE);
		}
		
		numberOfBytesWritten = write(fd, dataFromServer, bytesReceived);
		if(numberOfBytesWritten == -1)
		{
			perror("write()!");
			exit(EXIT_FAILURE);
		}
		
		if(sendto(clientFd, ack, sizeof(ack), 0,(struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
		{
			perror("sendto()!");
			exit(EXIT_FAILURE);
		}
		
	}	while(numberOfBytesWritten != 0);
}

void handler(int signum)
{
	close(clientFd);
	exit(EXIT_SUCCESS);
}