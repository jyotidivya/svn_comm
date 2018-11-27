/**
	code for connectionless server
 */

#include "server_client_header.h"


#define LENGTH_ARRAY     2048
#define DOMAIN			 AF_INET				//for IPv4 internet protocol
#define TYPE			 SOCK_DGRAM				//for connectionless communication
#define PROTOCOL		 0						//protocol value for internet protocol


int serverFd;

int main(int argc, char* argv[])
{
	char dataFromClient[LENGTH_ARRAY];
	char dataToClient[LENGTH_ARRAY];
	char serverIPAddress[30];
	char clientIPAddress[30];
	int serverPortNumber, max_fd, activity, clientPortNumber;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	int clientAddressLength = sizeof(clientAddress);
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
	if((serverFd = socket(DOMAIN, TYPE, PROTOCOL)) == -1)
	{
		perror("socket()!");
		exit(EXIT_FAILURE);
	}

	// assigning the address  specified  by  addr  to  the  socket  
	memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress . sin_family = DOMAIN;
	serverAddress . sin_port = htons(serverPortNumber);
	serverAddress . sin_addr . s_addr = inet_addr(serverIPAddress);

	clientAddress . sin_addr . s_addr = inet_addr(clientIPAddress);
	clientAddress . sin_port = htons(clientPortNumber);

	// binding server socket to specific port number and ip address
	if(bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}

	
	while(1)
	{
		// clear the socket set
		FD_ZERO(&readfds);

		// add server fd to set
		FD_SET(serverFd, &readfds);
		max_fd = serverFd;

		//add stdin to set
		FD_SET(fileno(stdin), &readfds); 

		// wait for an activity on one of the sockets

		activity = select (max_fd + 1, &readfds, NULL, NULL, &timeout);
		if(activity == -1)
		{
			perror("select()");
			exit(EXIT_FAILURE);
		}
		
		if(FD_ISSET(serverFd, &readfds))
		{
			memset(dataFromClient, '\0', sizeof(dataFromClient));
			if(recvfrom(serverFd, dataFromClient, sizeof(dataFromClient), 0, (struct sockaddr *)&clientAddress, &clientAddressLength) == -1)
			{
				perror("recvfrom()!");
				exit(EXIT_FAILURE);
			}
			puts(dataFromClient);
		}

		if(FD_ISSET(fileno(stdin), &readfds))
		{
			memset(dataToClient, '\0', sizeof(dataToClient));
			//send user inputted data to client
			
			if(gets(dataToClient) == NULL)
			{
				perror("gets()!");
				exit(EXIT_FAILURE);
			}
			
			if(sendto(serverFd, dataToClient, sizeof(dataToClient), 0,(struct sockaddr *)&clientAddress, sizeof(clientAddress)) == -1)
			{
				perror("sendto()!");
				exit(EXIT_FAILURE);
			}
			
		}

	} 


	return(EXIT_SUCCESS);
}

void handler(int signum)
{
	close(serverFd);
	exit(EXIT_SUCCESS);
}