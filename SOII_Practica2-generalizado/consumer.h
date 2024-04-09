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
#include "data.h"


void consumer(mem_shared *mem_map);


#endif //SOII_PRACTICA2_CONSUMER_H