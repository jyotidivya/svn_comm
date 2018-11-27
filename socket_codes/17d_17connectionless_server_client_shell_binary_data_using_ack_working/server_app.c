/**
	code for connectionless server
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
#define DOMAIN			 AF_INET				//for IPv4 internet protocol
#define TYPE			 SOCK_DGRAM				//for connectionless communication
#define PROTOCOL		 0						//protocol value for internet protocol
// #define IP_ADDRESS		 INADDR_ANY				//refers to any host

#define FILE_PATHNAME 			"/home/divya/Desktop/es_2Mbps_bbuny.h264"
#define FILE_OPEN_FLAGS			 O_RDONLY		
#define SIZE_TO_READ_FROM_FILE	 1024
#define ACKNOWLEDGMENT 			 "ACD0CF459ED23B10"

int serverFd, fd;

int main(int argc, char* argv[])
{
	char dataFromClient[LENGTH_ARRAY];
	char dataToClient[LENGTH_ARRAY];
	char serverIPAddress[30];
	char clientIPAddress[30];
	unsigned char ack[9];
	unsigned char acknowledgment[9];
	char hexString[] = ACKNOWLEDGMENT;
	char *pos = hexString;
	int serverPortNumber, clientPortNumber, numberOfBytesRead, bytesReceived,i;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	// int clientAddressLength = sizeof(clientAddress);

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
	if((serverFd = socket(DOMAIN, TYPE, PROTOCOL)) == -1)
	{
		perror("socket()!");
		exit(EXIT_FAILURE);
	}

	// assigning the address  specified  by  addr  to  the  socket  
	memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress . sin_family = DOMAIN;
	serverAddress . sin_port = htons(serverPortNumber);
	if(inet_aton(serverIPAddress, (struct in_addr *)&(serverAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}

	clientAddress . sin_family = DOMAIN;
	if(inet_aton(clientIPAddress, (struct in_addr *)&(clientAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}
	clientAddress . sin_port = htons(clientPortNumber);

	// binding socket to specific port number and ip address
	if(bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}

	// open binary file
	fd = open(FILE_PATHNAME, FILE_OPEN_FLAGS);
	if(fd == -1)
	{
		perror("open()failed!");
		exit(EXIT_FAILURE);
	}

	for(i=0;i<9;i++)
	{
		sscanf(pos, "%2hhx", &acknowledgment[i]);
		pos += 2;
	}

	do
	{
		
		// send data read from binary file
		memset(dataToClient, '\0', sizeof(dataToClient));
		
		numberOfBytesRead = read(fd, dataToClient, SIZE_TO_READ_FROM_FILE);
		if(numberOfBytesRead == -1)
		{
			perror("read() failed!");
			exit(EXIT_FAILURE);
		}

		if(sendto(serverFd, dataToClient, numberOfBytesRead, 0,(struct sockaddr *)&clientAddress, sizeof(clientAddress)) == -1)
		{
			perror("sendto()!");
			exit(EXIT_FAILURE);
		}

		// usleep(10000);
		if((bytesReceived = recvfrom(serverFd, ack, sizeof(ack), 0, NULL, NULL)) == -1)
		{
			perror("recvfrom()!");
			exit(EXIT_FAILURE);
		}
		
		// for(i=0;i < bytesReceived;i++)
		// 		printf("%x", ack[i]);

		if(strcmp(ack, acknowledgment) == 0)
		{
			// for(i=0;i < bytesReceived;i++)
			// 	printf("%x", ack[i]);

			// printf("\n");
			continue;
		}
		
	}	while(numberOfBytesRead != 0);

	// close the file descriptor
	if(close(fd) == -1)
	{
		perror("close()!");
		exit(EXIT_FAILURE);
	}

	return(EXIT_SUCCESS);
}

void handler(int signum)
{
	close(serverFd);
	close(fd);
	exit(EXIT_SUCCESS);
}