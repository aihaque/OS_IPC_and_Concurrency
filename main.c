#include <stdlib.h>
#include <netdb.h>
#include "list.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "keyboard.h"
#include "sender.h"
#include "screen.h"
#include "receiver.h"
#include <pthread.h> 

static int LOCALPORT;
static int REMOTEPORT;
static char* REMOTEIP;
int socketDescriptor;

static void bindSocket()
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET; 
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(LOCALPORT); 
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    int binding = bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin));

    if (socketDescriptor < 0)
    {
        perror("Socket Creation Failed!");
        exit(EXIT_FAILURE);
    }

    if (binding == -1)
    {
        perror("Binding Failed!");
        exit(EXIT_FAILURE);
    }
    return;
}

// Hint to get the IP https://stackoverflow.com/questions/20115295/how-to-print-ip-address-from-getaddrinfo
static void getIp(char *remote_name, char *remote_port)
{
	struct addrinfo hints; 
    struct addrinfo *result, *res;
    struct sockaddr_in *sockinfo;
    char *ip;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	if (getaddrinfo(remote_name, remote_port, &hints, &result) != 0)
	{
		fprintf(stderr, "Bad ip name please double check\n");
        exit(EXIT_FAILURE);
	}

	for (res = result; res != NULL; res = res->ai_next)
	{
		if (res->ai_addr->sa_family == AF_INET)
		{
			sockinfo = (struct sockaddr_in *)res->ai_addr;
			ip = inet_ntoa(sockinfo->sin_addr);
            REMOTEIP = ip;
			freeaddrinfo(result);
			return;
		}
	}
    printf("address error");
    exit(EXIT_FAILURE);
    return;
}

int main(int argc, char **args)
{
    char *localPort = args[1];
    char *remoteIp = args[2];
    char *remotePort = args[3];
    LOCALPORT = atoi(localPort);
    REMOTEPORT = atoi(remotePort);
    getIp(remoteIp,remotePort);
    bindSocket();

    List *send_list = List_create();
    List *receive_list = List_create();
    
    pthread_mutex_t localMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t remoteMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t producersMutex = PTHREAD_MUTEX_INITIALIZER;

    keyboard_init(send_list,receive_list,&localMutex,&producersMutex);
    Sender_init(socketDescriptor,REMOTEIP,REMOTEPORT,send_list,receive_list,&localMutex);
    Receiver_init(socketDescriptor,send_list,receive_list,&remoteMutex,&producersMutex);
    Screen_init(socketDescriptor,send_list,receive_list,&remoteMutex);

    keyboard_wait_to_finish();
    Sender_wait_to_finish();
    Receiver_wait_to_finish();
    Screen_wait_to_finish();
    
    pthread_mutex_destroy(&localMutex);
    pthread_mutex_destroy(&remoteMutex);
    pthread_mutex_destroy(&producersMutex);
    close(socketDescriptor);
    return 0;
}