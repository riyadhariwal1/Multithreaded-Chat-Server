#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include "list.h"

void* Printer_printMessage (void* s);
void Printer_init();
void Printer_shutdown(void);
void printer_signal();
void printer_set_message(List* serverPrinterList);
void printer_set_shutdown_msg();

#endif


