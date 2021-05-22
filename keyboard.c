#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>

#include "keyboard.h"
#include "client.h"
#include "server.h"
#include "list.h"
#include "printer.h"
#include "main.h"

static List* clientKeyboardList;
static pthread_t keyboardThread;
pthread_attr_t attrKeyboard;

static pthread_cond_t s_synOkKeyboard = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_syncOkMutextKeyboard = PTHREAD_MUTEX_INITIALIZER;

#define MSG_MAX_LEN 1024 

void Keyboard_init(void) {
	clientKeyboardList = List_create();
	pthread_attr_init(&attrKeyboard);
	pthread_create(&keyboardThread, &attrKeyboard, Keyboard_getUserInput, NULL);
}

void* Keyboard_shutdown() {
	char* msg_result;
	pthread_join(keyboardThread, (void**)&msg_result); 
	pthread_cancel(keyboardThread);
    return msg_result;
}

//bind error cannot assign requested address
void* Keyboard_getUserInput(void* unused) {

	while(1) {
		char* msg = malloc(sizeof(char*)*MSG_MAX_LEN);
	    fgets(msg, MSG_MAX_LEN, stdin);	
	    msg[MSG_MAX_LEN] = '\0';

	    if (*msg == '!') {
	    	set_message("!");
	    	client_signal();
	    	
			pthread_mutex_lock(&s_syncOkMutextKeyboard);
			{
				pthread_cond_wait(&s_synOkKeyboard, &s_syncOkMutextKeyboard);
			}
			pthread_mutex_unlock(&s_syncOkMutextKeyboard);
			pthread_exit(NULL);
			return NULL;
	    }

	    int prependSucess = List_prepend(clientKeyboardList, msg);
	    if (prependSucess == -1) {
	    	printf("--list is full--\n");
	    } 
	    
		get_message(clientKeyboardList);
		client_signal();

		pthread_mutex_lock(&s_syncOkMutextKeyboard);
	    {
	        pthread_cond_wait(&s_synOkKeyboard, &s_syncOkMutextKeyboard);
	    }
	    pthread_mutex_unlock(&s_syncOkMutextKeyboard);
		free(msg);
	}

	pthread_exit(NULL);
	return NULL;
}

void keyboard_signal() {
    pthread_mutex_lock(&s_syncOkMutextKeyboard);
    {
        pthread_cond_signal(&s_synOkKeyboard);
    }
    pthread_mutex_unlock(&s_syncOkMutextKeyboard);
}