#ifndef SOII_PRACTICA2_PRODUCER_H
#define SOII_PRACTICA2_PRODUCER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include "time.h"
#include "data.h"



#define get_rand() ((int)rand()%10)



void* producer(void* args); /* Ya no hace falta pasarle el tamaño, está declarado aquí */



#endif //SOII_PRACTICA2_PRODUCER_H
