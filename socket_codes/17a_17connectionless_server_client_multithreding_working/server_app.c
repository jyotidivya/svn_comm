/**
	code for connectionless server
 */

#include "server_client_header.h"



#define DOMAIN			 AF_INET				//for IPv4 internet protocol
#define TYPE			 SOCK_DGRAM				//for connectionless communication
#define PROTOCOL		 0						//protocol value for internet protocol


int serverFd;
int Rretval = 4, Sretval = 3;
int main(int argc, char* argv[])
{
	
	char serverIPAddress[30];
	char clientIPAddress[30];
	int serverPortNumber, max_fd, activity, clientPortNumber;
	struct sockaddr_in serverAddress;
	struct argumentToThread argument;
	struct timeval timeout;
	timeout . tv_sec = 3;
	timeout . tv_usec = 0;
	pthread_t stid, rtid;	

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

	// assigning the server address  specified  by  addr  to  the  socket  
	memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress . sin_family = DOMAIN;
	serverAddress . sin_port = htons(serverPortNumber);
	if(inet_aton(serverIPAddress, (struct in_addr *)&(serverAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}	

	// assigning the client address to  the  socket  
	argument . clientAddress . sin_family = DOMAIN;
	if(inet_aton(clientIPAddress, (struct in_addr *)&(argument . clientAddress . sin_addr . s_addr)) == 0)
	{
		perror("inet_aton()!");
		exit(EXIT_FAILURE);
	}
	argument . clientAddress . sin_port = htons(clientPortNumber);
	argument . clientAddressLength = sizeof(argument . clientAddress);

	// binding server socket to specific port number and ip address
	if(bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("bind()!");
		exit(EXIT_FAILURE);
	}

		
	if(pthread_create(&stid, NULL, (void * (*)(void *))&sender_thread, (void *)&argument) != 0)
	{
		perror("sender's pthread_create()!");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&rtid, NULL, (void * (*)(void *))&receiver_thread, (void *)&argument) != 0)
	{
		perror("receiver's pthread_create()!");
		exit(EXIT_FAILURE);
	}

	if(pthread_join(stid, (void**)&Sretval) != 0)
	{
		perror("sender's pthread_join()!");
		exit(EXIT_FAILURE);
	}

	if(pthread_join(rtid, (void**)&Rretval) != 0)
	{
		perror("receiver's pthread_join()!");
		exit(EXIT_FAILURE);
	}	

	return(EXIT_SUCCESS);
}

void handler(int signum)
{
	close(serverFd);
	exit(EXIT_SUCCESS);

	pthread_exit((void*)&Sretval);
	pthread_exit((void*)&Rretval);
}

void* sender_thread(void *args)
{
	int length;
	struct argumentToThread *argument = (struct argumentToThread *)args;
	while(1)
	{
		memset(argument -> dataToClient, '\0', sizeof(argument -> dataToClient));
		
		//send user inputted data to client
		if(fgets(argument -> dataToClient, LENGTH_ARRAY, stdin) == NULL)
		{
			perror("gets()!");
			exit(EXIT_FAILURE);
		}

		length = strlen(argument -> dataToClient);
		if(argument -> dataToClient[length-1] == '\n')
		{
			argument -> dataToClient[--length] = '\0';
		}
		
		if(sendto(serverFd, argument -> dataToClient, sizeof(argument -> dataToClient), 0,(struct sockaddr *)&(argument -> clientAddress), sizeof(argument -> clientAddress)) == -1)
		{
			perror("sendto()!");
			exit(EXIT_FAILURE);
		}
	}

	// pthread_exit((void*)&retval);

}

void* receiver_thread(void* args)
{
	int retval = 4;
	struct argumentToThread *argument = (struct argumentToThread *)args;
	while(1)
	{		
		memset(argument -> dataFromClient, '\0', sizeof(argument -> dataFromClient));
		if(recvfrom(serverFd, argument -> dataFromClient, sizeof(argument -> dataFromClient), 0, NULL, NULL) == -1)
		{
			perror("recvfrom()!");
			exit(EXIT_FAILURE);
		}
		puts(argument -> dataFromClient);
	}
	pthread_exit((void*)&retval);
}