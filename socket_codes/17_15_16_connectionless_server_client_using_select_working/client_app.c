/**
	code for connectionless client
 */

#include "server_client_header.h"

// #define SERVER_PORT      3555
#define LENGTH_ARRAY     1024
#define DOMAIN			 AF_INET			//for IPv4 internet protocol
#define TYPE			 SOCK_DGRAM			//for connectionless communication
#define PROTOCOL		 0					//protocol value for internet protocol

int clientFd;

int main(int argc, char *argv[])
{
	int serverPortNumber, activity, max_fd, clientPortNumber;
	char serverIPAddress[30];
	char clientIPAddress[30];
	char dataToServer[LENGTH_ARRAY] = "send data";
	char dataFromServer[LENGTH_ARRAY];
	struct sockaddr_in serverAddress, clientAddress;
	int serverAddressLength = sizeof(serverAddress);
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

	memset((struct sockaddr_in *)&clientAddress, 0, sizeof(clientAddress));
	clientAddress . sin_family = DOMAIN;
	clientAddress . sin_addr . s_addr = inet_addr(clientIPAddress);
	clientAddress . sin_port = htons(clientPortNumber);

	serverAddress . sin_port = htons(serverPortNumber);
	serverAddress . sin_addr . s_addr = inet_addr(serverIPAddress);

	
	// binding client socket to particular port and address
	if(bind(clientFd, (struct sockaddr *)&clientAddress, sizeof(clientAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}
	// send request to server to send data

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
			if(gets(dataToServer) == NULL)
			{
				perror("gets()!");
				exit(EXIT_FAILURE);
			}
			
			if(sendto(clientFd, dataToServer, sizeof(dataToServer), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress) ) == -1)
			{
				perror("sendto()!");
				exit(EXIT_FAILURE);
			}
		}

		if(FD_ISSET(clientFd, &readfds))
		{
			// receive data from the server
			memset(dataFromServer, '\0', sizeof(dataFromServer));
			if(recvfrom(clientFd, dataFromServer, sizeof(dataFromServer), 0, (struct sockaddr *)&serverAddress, &serverAddressLength) == -1)
			{
				perror("recvfrom()!");
				exit(EXIT_FAILURE);
			}

			puts(dataFromServer);
		}

	}
}

void handler(int signum)
{
	close(clientFd);
	exit(EXIT_SUCCESS);
}