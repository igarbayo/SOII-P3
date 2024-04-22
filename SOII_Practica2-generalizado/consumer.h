#ifndef SOII_PRACTICA2_CONSUMER_H
#define SOII_PRACTICA2_CONSUMER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "data.h"

/**
 * Función de comportamiento de un consumidor
 * @param args argumentos a pasar con el formato para un hilo
 */
void* consumer(void* args);


#endif //SOII_PRACTICA2_CONSUMER_H
