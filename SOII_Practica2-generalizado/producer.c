#include "producer.h"

extern mem_shared * mem_map;


void contribute(){

}

int produce_item(){
    return(get_rand());
}

void insert_item(int item) {
    struct timeval tv;

    /* Inicializamos la semilla */
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    /* Insertamos en la posición correspondiente */
    mem_map->buffer[mem_map->count] = item;

    /* Actualizamos count */
    mem_map->count++;

    /* Printeamos el buffer para constatar posibles carreras críticas */
    printf("Buffer(PRODUCTOR : %d):\t| %d | %d | %d | %d | %d | %d | %d | %d |\n", (int)pthread_self(), mem_map->buffer[0], mem_map->buffer[1], mem_map->buffer[2], mem_map->buffer[3],
           mem_map->buffer[4], mem_map->buffer[5],mem_map->buffer[6], mem_map->buffer[7]);

}


void* producer(){
    int item; /* Elemento a extraer del buffer */
    int i=0; /* Contador del bucle*/

    while(i<BUC_PROD){

        /* Metemos un sleep fuera de la región crítica */
        sleep(rand()%3);

        item = produce_item(); /* Producimos un entero aleatorio entre 0 y 10 */
        pthread_mutex_lock(&mem_map->mutex);
        while(mem_map->count == N) pthread_cond_wait(&mem_map->cond_producer, &mem_map->mutex); // Mientras esté lleno.
        insert_item(item); /* insertamos el item en el buffer */
        pthread_cond_signal(&mem_map->cond_consumer);
        pthread_mutex_unlock(&mem_map->mutex);




        i++; /* Aumentamos el contador */
    }
}
