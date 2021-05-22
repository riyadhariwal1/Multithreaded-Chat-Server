#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <inttypes.h>
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>

#include "client.h"
#include "server.h"
#include "keyboard.h"
#include "list.h"
#include "main.h"
#include "printer.h"
  
#define MSG_MAX_LEN 1024 

pthread_t threadClient;
pthread_attr_t attrClient;

static pthread_cond_t s_synOkClient = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_syncOkMutextClient = PTHREAD_MUTEX_INITIALIZER;
static char* message;

List* list;

static int hostPort;
static int remotePort;
static char* ip;

void* client(void* unused) { 

    static int socketDescriptor; 

    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, &remotePort, &hints, &servinfo);

    // int status;

    // try getting this to work
    // if ((status = getaddrinfo(ip, &remotePort, &hints, &servinfo)) != 0) {
    //     fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    //     exit(1);
    // }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl((intptr_t)servinfo);
    sin.sin_port = htons(remotePort);

    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    while (1) {
        
        //Wait until signalled
        pthread_mutex_lock(&s_syncOkMutextClient);
        {
            pthread_cond_wait(&s_synOkClient, &s_syncOkMutextClient);
        }
        pthread_mutex_unlock(&s_syncOkMutextClient);

        int n = sendto(socketDescriptor, (const char *)message, strlen(message), 
            MSG_CONFIRM, (const struct sockaddr *) &sin, sizeof(sin)); 

        if (*message == '!') {
            changeLoop();
            printer_set_shutdown_msg();
            printer_signal();
            keyboard_signal();
            freeaddrinfo(servinfo);
            close(socketDescriptor); 
            pthread_exit(NULL);
            return NULL;
        }

        if (n < 0){
            printf("sendto failed\n");
        } 
        
        keyboard_signal();
    }
    close(socketDescriptor); 
    pthread_exit(NULL);
    return NULL;
} 

void client_signal() {
    pthread_mutex_lock(&s_syncOkMutextClient);
    {
        pthread_cond_signal(&s_synOkClient);
    }
    pthread_mutex_unlock(&s_syncOkMutextClient);
}

void client_init(int myPort, char* otherIP, int otherPort) {
    pthread_attr_init(&attrClient);
    pthread_create(&threadClient, &attrClient, client, NULL);
    hostPort = myPort;
    remotePort = otherPort;
    ip = otherIP;
}

void client_waitForShutdown() {
    pthread_join(threadClient, NULL);
    pthread_cancel(threadClient);
}

void get_message(List* clientKeyboardList) {
    list = clientKeyboardList;
    message = (char*)List_trim(clientKeyboardList);
}

void set_message(char* msg) {
    message = msg;
}