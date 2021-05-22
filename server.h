#ifndef _SERVER_H_
#define _SERVER_H_

#include <pthread.h>

void* server(void* unused);
void server_signal();
void server_init(int myPort, char* otherIP, int otherPort);
void server_waitForShutdown();
void server_setShutDown();
void changeLoop();


#endif