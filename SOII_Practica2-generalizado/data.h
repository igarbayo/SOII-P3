#ifndef SOII_PRACTICA2_DATA_H
#define SOII_PRACTICA2_DATA_H



#define N 12
#define BUC 10
#define BUC_PROD 18
#define P 8
#define C 6
#define TAM 110*(P+C) // Tamaño del buffer T


typedef struct{
    int buffer[N];
    int T[TAM];
    int count;
    int index_sum;
    pthread_mutex_t mutex;
    pthread_cond_t cond_consumer;
    pthread_cond_t cond_producer;
}mem_shared;


#endif //SOII_PRACTICA2_DATA_H
