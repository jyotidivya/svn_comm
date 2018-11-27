/**
 	including library and declaring the functions for socket programming
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
#include <pthread.h>

#define LENGTH_ARRAY     2048

// structure to pass the argument to thread
struct argumentToThread
{
	char dataFromClient[LENGTH_ARRAY];
	char dataToClient[LENGTH_ARRAY];
	struct sockaddr_in clientAddress;
	int clientAddressLength;
};

void handler(int);
void* sender_thread(void *);
void* receiver_thread(void *);

