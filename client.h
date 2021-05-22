#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>
#include "list.h"

void* client(void* unused);
void client_signal();
void client_init(int myPort, char* otherIP, int otherPort);
void client_waitForShutdown();
void set_message(char* msg);
void get_message(List* clientKeyboardList);
void client_setShutDown();

#endif