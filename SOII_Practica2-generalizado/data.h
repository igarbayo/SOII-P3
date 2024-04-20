#ifndef SOII_PRACTICA2_DATA_H
#define SOII_PRACTICA2_DATA_H



#define N 12 /* Número de elementos del buffer */
#define BUC_PROD 18 /*Número de iteraciones del productor */
#define BUC_CONS ((int) (P * BUC_PROD) / C) /* Número de iteraciones del consumidor */
#define P 8 /* Número de productores */
#define C 6 /* Número de consumidores */
#define TAM 110*(P+C) /* Tamaño del buffer T */


typedef struct{
    int buffer[N]; /* Búfer del problema productor-consumidor */
    int T[TAM]; /* Array sobre el que se calculará la suma */
    int count; /* Contador usado como índice para la inserción/eliminación en el búfer del productor/consumidor */
    int odd_sum; /* Suma realizada sobre las posiciones impares del array T */
    int even_sum; /* Suma realizada sobre las posiciones pares del array T */
    int index_odd_sum; /* Índice usado para las suma de los números impares */
    int index_even_sum; /* Índice usado para las suma de los números pares */
    int flag[P]; /* Flag para la deteccion de cuando han terminado de PRODUCIR todos los productores */
    pthread_mutex_t mutex; /* Mutex problema del productor consumidor */
    pthread_mutex_t mutex_odd_sum; /* Mutex suma impares */
    pthread_mutex_t mutex_even_sum; /* Mutex suma pares  */
    pthread_cond_t cond_consumer; /* Variable de condición del consumidor */
    pthread_cond_t cond_producer; /* Variable de condición del productor */
}mem_shared;


#endif //SOII_PRACTICA2_DATA_H
