#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>

#include "printer.h"
#include "server.h"
#include "list.h"

static List* senderPrinterList;
static pthread_t printerThread;
static pthread_cond_t s_synOkPrinter = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_syncOkMutextPrinter = PTHREAD_MUTEX_INITIALIZER;

static char* message;

void* Printer_printMessage (void* unused) {
	while(1) {	
		pthread_mutex_lock(&s_syncOkMutextPrinter);
        {
            pthread_cond_wait(&s_synOkPrinter, &s_syncOkMutextPrinter);
        }
        pthread_mutex_unlock(&s_syncOkMutextPrinter);

        if (*message == '!') {
            pthread_exit(NULL);
            return NULL;
        }
	    printf("client said: %s", message);
	}
    pthread_exit(NULL);
    return NULL;
}

void Printer_init() {
    senderPrinterList = List_create();
    pthread_create(&printerThread, NULL, Printer_printMessage, NULL);
}

void Printer_shutdown(void){
    pthread_join(printerThread, NULL);
    pthread_cancel(printerThread);
}

void printer_signal() {
    pthread_mutex_lock(&s_syncOkMutextPrinter);
    {
        pthread_cond_signal(&s_synOkPrinter);
    }
    pthread_mutex_unlock(&s_syncOkMutextPrinter);
}

void printer_set_message(List* serverPrinterList) {
    message = (char*)List_trim(serverPrinterList);
}

void printer_set_shutdown_msg() {
    message = "!";
}