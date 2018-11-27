/**
	code for connection oriented socket for chat application between server and client 
	this is server code.
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


#define LENGTH_ARRAY     2048
#define DOMAIN			 AF_INET				//for IPv4 internet protocol
#define TYPE			 SOCK_STREAM			//for connection oriented communication
#define PROTOCOL		 0						//protocol value for internet protocol

#define LEVEL			SOL_SOCKET						 // to manipulate options at socket API level
#define OPTNAME			SO_REUSEADDR | SO_REUSEPORT		 // option name
#define BACKLOG			3							    //maximum length to which queue of pending connections may grow


int serverFd, acceptedFd;

int main(int argc, char* argv[])
{
	char dataFromClient[LENGTH_ARRAY];
	char dataToClient[LENGTH_ARRAY];
	char serverIPAddress[30];
	char clientIPAddress[30];
	int serverPortNumber, max_fd, activity, optval = 1, length, numberOfBytes, numberOfBytesSent, clientPortNumber;
	struct sockaddr_in serverAddress;
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

	// set options on sockets
	if(setsockopt(serverFd, LEVEL, OPTNAME, &optval, sizeof(optval)) == -1)
	{
		perror("setsockopt()!");
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

	
	// binding server socket to specific port number and ip address
	if(bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}

	// listen for any connection
againListen:	if(listen(serverFd, BACKLOG) == -1)
				{
					perror("listen()!");
					exit(EXIT_FAILURE);
				}

	// accept the connection
	if ((acceptedFd = accept(serverFd, NULL, NULL)) < 0)
    {
        perror("accept()!");
        exit(EXIT_FAILURE);
    }

    // printf("server fd:%d\n", serverFd);
    // printf("accepted fd fd:%d\n", acceptedFd);
    // printf("server fd:%d\n", serverFd);

	// printf("comm starts\n");
	while(1)
	{
		// clear the socket set
		FD_ZERO(&readfds);

		// add server fd to set
		FD_SET(acceptedFd, &readfds);
		max_fd = acceptedFd;

		//add stdin to set
		FD_SET(fileno(stdin), &readfds); 

		// wait for an activity on one of the sockets

		activity = select (max_fd + 1, &readfds, NULL, NULL, &timeout);
		if(activity == -1)
		{
			perror("select()");
			exit(EXIT_FAILURE);
		}
		
		if(FD_ISSET(acceptedFd, &readfds))
		{
			memset(dataFromClient, '\0', sizeof(dataFromClient));
			if((numberOfBytes = recv(acceptedFd, dataFromClient, sizeof(dataFromClient), 0)) == -1)
			{
				perror("recv()!");
				exit(EXIT_FAILURE);
			}
			
			// printf("%s\n",dataFromClient);
			// printf("bytes recv:%d\n", numberOfBytes);
			
			if(numberOfBytes == 0)
			{
				close(acceptedFd);
				goto againListen;
			}

			printf("%s\n",dataFromClient);
		}

		if(FD_ISSET(fileno(stdin), &readfds))
		{
			memset(dataToClient, '\0', sizeof(dataToClient));
			
			//send user inputted data to client
			if(fgets(dataToClient, sizeof(dataToClient), stdin) == NULL)
			{
				perror("fgets()!");
				exit(EXIT_FAILURE);
			}

			length = strlen(dataToClient);
			if(dataToClient[length-1] == '\n')
			{
				dataToClient[--length] = '\0';
			}
			
			// printf("data to send:%s\n", dataToClient);
			// printf("length of data:%d\n", length);

			if((numberOfBytesSent = send(acceptedFd, dataToClient, strlen(dataToClient), 0)) == -1)
			{
				perror("sendto()!");
				exit(EXIT_FAILURE);
			}

			// printf("sent data length:%d\n", numberOfBytesSent);
			
		}

	} 


	return(EXIT_SUCCESS);
}

void handler(int signum)
{
	close(serverFd);
	close(acceptedFd);
	exit(EXIT_SUCCESS);
}