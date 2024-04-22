#ifndef SOII_PRACTICA2_DATA_H
#define SOII_PRACTICA2_DATA_H



#define N 12 /* Tamaño del buffer */
#define BUC_PROD 18 /* Número de iteraciones del bucle de los productores (número de elementos producidos) */
#define P 8 /* Número de productores */
#define C 6 /* Nümero de consumidores */
#define TAM 110*(P+C) // Tamaño del buffer T


typedef struct{
    int buffer[N]; /* Búfer del problema productor-consumidor */
    int T[TAM]; /* Array sobre el que se calculará la suma */
    int count; /* Contador usado como índice para la inserción/eliminación en el búfer del productor/consumidor */
    int odd_sum; /* Suma realizada sobre las posiciones impares del array T */
    int even_sum; /* Suma realizada sobre las posiciones pares del array T */
    int index_odd_sum; /* Índice usado para las suma de los números impares */
    int index_even_sum; /* Índice usado para las suma de los números pares */
    int not_finish; /* Flag para que los consumidores iteren hasta que se acaben todos los items producidos */
    pthread_mutex_t mutex; /* Mutex problema del productor consumidor */
    pthread_mutex_t mutex_odd_sum; /* Mutex suma impares */
    pthread_mutex_t mutex_even_sum; /* Mutex suma pares  */
    sem_t* empty; /* Semáforo usado en vez de la variable ed condición para los consumidores */
    sem_t* full; /* Semáforo usado en vez de la variable ed condición para los productores */
}mem_shared;


#endif //SOII_PRACTICA2_DATA_H
