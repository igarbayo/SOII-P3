#ifndef SOII_PRACTICA2_DATA_H
#define SOII_PRACTICA2_DATA_H

#include <semaphore.h>

#define N 8
#define BUC 10

typedef struct{
    int buffer[N];
    sem_t* mutex;
    sem_t* empty;
    sem_t* full;
}mem_shared;


#endif //SOII_PRACTICA2_DATA_H
