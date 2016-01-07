#ifndef ECOUTE_H
#define ECOUTE_H

#define PORT 1234

#include <pthread.h>
#include "default_lib.h"

void * ecoute_thread_client();
int ecoute_client();
void ecoute_kill_thread();

pthread_t thread_ecoute;

#endif