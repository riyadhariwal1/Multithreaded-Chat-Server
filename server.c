#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <netdb.h>

#include "server.h"
#include "printer.h"
#include "list.h"
#include "keyboard.h"
#include "main.h"
#include "client.h"
  
#define MSG_MAX_LEN 1024 

pthread_t threadServer;
pthread_attr_t attrServer;

static List* serverPrinterList;
static bool loop = true;

static int hostPort;
static int remotePort;
static char* ip;

// Driver code 
void* server(void* unused) { 

    static int socketDescriptor;
    struct sockaddr_in serverSocket;

    struct addrinfo* servinfo;

    struct sockaddr_in *sinRemote;
    memset(&sinRemote, 0, sizeof(sinRemote));

    socketDescriptor = getaddrinfo(ip, &hostPort, NULL, &servinfo);

    memset(&serverSocket, 0, sizeof(serverSocket));
    serverSocket.sin_family = AF_INET;                   
    serverSocket.sin_addr.s_addr = htonl(INADDR_ANY);  
    serverSocket.sin_port = htons(hostPort);          

    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    bind(socketDescriptor, (struct sockaddr*) &serverSocket, sizeof(serverSocket));

    while (loop) {

        char messageRx[MSG_MAX_LEN]; 
        unsigned int sin_len = sizeof(sinRemote);

        int n = recvfrom(socketDescriptor, messageRx, MSG_MAX_LEN,  
                    0, ( struct sockaddr *) &sinRemote, &sin_len);

        if (n < 0){
            printf("recieved failed\n");
        } 

        if (messageRx[0] == '!') {
            printer_set_shutdown_msg();
            printer_signal();
            freeaddrinfo(servinfo);
            close(socketDescriptor); 
            pthread_exit(NULL);
            return NULL;
        }

        messageRx[n] = '\0'; 
        int prependSucess = List_prepend(serverPrinterList, messageRx);
        if (prependSucess == -1) {
            printf("--list is full--\n");
        }
        printer_set_message(serverPrinterList);
        printer_signal();
    }

    pthread_exit(NULL);
    return NULL;
} 

void server_init(int myPort, char* otherIP, int otherPort) {
    serverPrinterList = List_create();
    pthread_attr_init(&attrServer);
    pthread_create(&threadServer, &attrServer, server, NULL);
    hostPort = myPort;
    remotePort = otherPort;
    ip = otherIP;
}

void server_waitForShutdown() {
    pthread_join(threadServer, NULL);
    pthread_cancel(threadServer);
}

pthread_t* server_thread() {
    return &threadServer;
}

void changeLoop() {
    loop = false;
}