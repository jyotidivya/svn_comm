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
#include <sys/stat.h>
#include <fcntl.h>


void handler(int);


#define LENGTH_ARRAY     2048
#define DOMAIN			 AF_INET				//for IPv4 internet protocol
#define TYPE			 SOCK_STREAM			//for connection oriented communication
#define PROTOCOL		 0						//protocol value for internet protocol

#define LEVEL			SOL_SOCKET						 // to manipulate options at socket API level
#define OPTNAME			SO_REUSEADDR | SO_REUSEPORT		 // option name
#define BACKLOG			3							    //maximum length to which queue of pending connections may grow

#define FILE_PATHNAME 			"/home/divya/Desktop/es_2Mbps_bbuny.h264"
#define FILE_OPEN_FLAGS			 O_RDONLY		
#define SIZE_TO_READ_FROM_FILE	 1024

int serverFd, acceptedFd;

int main(int argc, char* argv[])
{
	// char dataFromClient[LENGTH_ARRAY];
	char dataToClient[LENGTH_ARRAY];
	char serverIPAddress[30];
	char clientIPAddress[30];
	int serverPortNumber, optval = 1, clientPortNumber, fileFd, numberOfBytesRead;
	struct sockaddr_in serverAddress;
	
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
	if(listen(serverFd, BACKLOG) == -1)
	{
		perror("listen()!");
		exit(EXIT_FAILURE);
	}

	// accept the connection
	if((acceptedFd = accept(serverFd, NULL, NULL)) < 0)
    {
        perror("accept()!");
        exit(EXIT_FAILURE);
    }

    // opening binary file
    fileFd = open(FILE_PATHNAME, FILE_OPEN_FLAGS);
	if(fileFd == -1)
	{
		perror("open()failed!");
		exit(EXIT_FAILURE);
	}

	printf("file opened\n");

	do
	{
		// send data read from binary file
		memset(dataToClient, '\0', sizeof(dataToClient));
		
		numberOfBytesRead = read(fileFd, dataToClient, SIZE_TO_READ_FROM_FILE);
		if(numberOfBytesRead == -1)
		{
			perror("read() failed!");
			exit(EXIT_FAILURE);
		}

		if(send(acceptedFd, dataToClient, numberOfBytesRead, 0) == -1)
		{
			perror("sendto()!");
			exit(EXIT_FAILURE);
		}
		

	}	while(numberOfBytesRead != 0);

	


	return(EXIT_SUCCESS);
}

void handler(int signum)
{
	close(serverFd);
	close(acceptedFd);
	exit(EXIT_SUCCESS);
}