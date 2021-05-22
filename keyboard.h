#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <pthread.h>

void Keyboard_init();
void* Keyboard_getUserInput(void* unused);
void* Keyboard_shutdown();
void keyboard_signal();
void keyboard_setShutDown();

#endif