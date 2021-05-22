#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include "client.h"
#include "server.h"
#include "keyboard.h"
#include "printer.h"

#define MSG_MAX_LEN 1024 

int set_shutDown(){

    server_waitForShutdown();
    client_waitForShutdown();
    Keyboard_shutdown();
    Printer_shutdown();
    
    printf("\n\nDONE\n");

    return 0;
}

int main(int argc, char** args)
{
    if (argc < 4) {
		printf("Missing arguments\n");
		exit(-1);
	}

    int myPort = atoi(args[1]);
    char* remoteIP = args[2];
    int remotePort = atoi(args[3]);

    server_init(myPort, remoteIP, remotePort);
    client_init(myPort, remoteIP, remotePort);
    Printer_init();
    Keyboard_init();

    set_shutDown();

    return 0;
}
