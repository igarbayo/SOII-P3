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
#include "time.h"
#include "data.h"
#include <sys/time.h>


#define get_rand() ((int)rand()%11)



void producer(mem_shared *mem_map); /* Ya no hace falta pasarle el tamaño, está declarado aquí */



#endif //SOII_PRACTICA2_PRODUCER_H
