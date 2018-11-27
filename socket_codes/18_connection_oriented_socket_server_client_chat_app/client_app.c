/**
	code for connection oriented socket for chat application between server and client
	this is client code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>


void handler(int);

// #define SERVER_PORT      3555
#define LENGTH_ARRAY     1024

#define DOMAIN			 AF_INET			//for IPv4 internet protocol
#define TYPE			 SOCK_STREAM			//for connection oriented communication
#define PROTOCOL		 0					//protocol value for internet protocol

#define OPTNAME			SO_REUSEADDR | SO_REUSEPORT		 // option name
#define LEVEL			SOL_SOCKET						 // to manipulate options at socket API level

int clientFd;

int main(int argc, char *argv[])
{
	int serverPortNumber, activity, max_fd, length, numberOfBytesReceived, numberOfBytesSent, clientPortNumber, optval = 1;
	char serverIPAddress[30];
	char clientIPAddress[30];
	char dataToServer[LENGTH_ARRAY];
	char dataFromServer[LENGTH_ARRAY];
	struct sockaddr_in serverAddress, clientAddress;
	struct timeval timeout;
	timeout . tv_sec = 3;
	timeout . tv_usec = 0;

	// set of socket descriptors
	fd_set readfds;

	if(argc < 5)
	{
		printf("usage: enter <Server IP Address> <server port number> <client IP Address> <client port number>\n");
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

	// printf("client fd:%d\n", clientFd);

	// set options on sockets
	if(setsockopt(clientFd, LEVEL, OPTNAME, &optval, sizeof(optval)) == -1)
	{
		perror("setsockopt()!");
		exit(EXIT_FAILURE);
	}

	memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress . sin_family = DOMAIN;
	serverAddress . sin_port = htons(serverPortNumber);
	if(inet_aton(serverIPAddress, (struct in_addr *)&(serverAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}

	memset((struct sockaddr_in *)&clientAddress, 0, sizeof(clientAddress));
	clientAddress . sin_family = DOMAIN;
	clientAddress . sin_port = htons(clientPortNumber);
	if(inet_aton(clientIPAddress, (struct in_addr *)&(clientAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}

	// binding client socket to specific port number and ip address
	if(bind(clientFd, (struct sockaddr *)&clientAddress, sizeof(clientAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}

	// connect to server
	if(connect(clientFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("connect()!");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		// clear the socket set
		FD_ZERO(&readfds);

		// add server fd to set
		FD_SET(clientFd, &readfds);
		max_fd = clientFd;

		//add stdin to set
		FD_SET(fileno(stdin), &readfds); 

		// wait for an activity on one of the sockets

		activity = select (max_fd + 1, &readfds, NULL, NULL, &timeout);
		if(activity == -1)
		{
			perror("select()");
			exit(EXIT_FAILURE);
		}
		
		if(FD_ISSET(fileno(stdin), &readfds))
		{
			memset(dataToServer, '\0', sizeof(dataToServer));
			
			// send data to server
			if(fgets(dataToServer, sizeof(dataToServer), stdin) == NULL)
			{
				perror("fgets()!");
				exit(EXIT_FAILURE);
			}
			
			length = strlen(dataToServer);
			if(dataToServer[length-1] == '\n')
			{
				dataToServer[--length] = '\0';
			}

			// printf("data sent:%s\n", dataToServer);
			// printf("length of data:%d\n", length);


			if((numberOfBytesSent = send(clientFd, dataToServer, strlen(dataToServer), 0)) == -1)
			{
				perror("send()!");
				exit(EXIT_FAILURE);
			}

			// printf("length of data sent:%d\n", numberOfBytesSent);

		}

		if(FD_ISSET(clientFd, &readfds))
		{
			// receive data from the server
			memset(dataFromServer, '\0', sizeof(dataFromServer));
			if((numberOfBytesReceived = recv(clientFd, dataFromServer, sizeof(dataFromServer), 0)) == -1)
			{
				perror("recvfrom()!");
				exit(EXIT_FAILURE);
			}
			
			// printf("%s\n", dataFromServer);
			
			if(numberOfBytesReceived == 0)
			{
				printf("server shut down the connection\n");
				exit(EXIT_SUCCESS);
			}
			printf("%s\n", dataFromServer);

		}

	}
}

void handler(int signum)
{
	close(clientFd);
	exit(EXIT_SUCCESS);
}