#include "consumer.h"
#include "producer.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>


mem_shared * mem_map; // Memoria compartida con el mutex y los cond_t

int main(int argc, char** argv) {
    pthread_t consumer_ids[C]; // Array con los ids de los consumidores
    pthread_t producer_ids[P]; // Array con los ids de los productores
    int i, j;

    /* Inicializamos la estructura con el buffer */
    mem_map = (mem_shared*) malloc (sizeof(mem_shared));
    mem_map->count=0;

    /* Inicializamos el buffer a -1 */
    for (i=0; i<N; i++) {
        mem_map->buffer[i]=-1;
    }

    /* Inicialización de mutex y variables de condición */
    pthread_mutex_init(&mem_map->mutex, 0);

    pthread_cond_init(&mem_map->cond_consumer, 0);

    pthread_cond_init(&mem_map->cond_producer, 0);

    /* Inicilizamos el vector T */
    for(i = 0; i < TAM; i++){
        mem_map->T[i] = (int) i/2; // El cast redondea a la baja
    }

    /* Crear consumidores */
    for (j = 0; j < C; j++){
        if((pthread_create(&consumer_ids[j], NULL, consumer, NULL)) != 0){
            printf("Error creando el hilo consumidor %d\n", j);
        }
        //printf("\n Proceso CONSUMIDOR [%d] acabando...\n", getpid());
    }

    /* Crear productores */
    for (j = 0; j < P; j++){
        if((pthread_create(&producer_ids[j], NULL, producer, NULL)) != 0){
            printf("Error creando el hilo productor %d\n", j);
        }
        //printf("\n Proceso PRODUCTOR [%d] acabando...\n", getpid());
    }

    /* Esperamos a que acaben los consumidores */
    for (j=0; j<C; j++) {
        pthread_join(consumer_ids[j], NULL);
    }

    /* Esperamos a que acaben los productores */
    for (j=0; j<P; j++) {
        pthread_join(producer_ids[j], NULL);
    }

    /* Cierre de mutex y variables de condición */
    pthread_cond_destroy(&mem_map->cond_consumer);
    pthread_cond_destroy(&mem_map->cond_producer);
    pthread_mutex_destroy(&mem_map->mutex);

    /* Liberamos la proyección de memoria */
    free(mem_map);

    return 0;
}
