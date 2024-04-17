#include "consumer.h"
#include "producer.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>


mem_shared * mem_map; // Memoria compartida con el mutex y los cond_t



int main(int argc, char** argv) {
    pthread_t consumer_ids[C]; // Array con los ids de los consumidores
    pthread_t producer_ids[P]; // Array con los ids de los productores
    int i=0, j;
    int sum_truth=0;

    /* Inicializamos la estructura con el buffer */
    mem_map = (mem_shared*) malloc (sizeof(mem_shared));

    //mem_map->BUC_CONS = (int) (P * BUC_PROD) / C;
    //mem_map->BUC_CONS++; /* Sumamos una iteración más ya que el casteo redondea a la baja */

    // Cuenta a 0
    mem_map->count=0;

    // Para que sume pares
    mem_map->index_odd_sum = 1;

    // Para que sume impares
    mem_map->index_even_sum = 0;

    /* Inicializamos el buffer a -1 */
    for (i=0; i<N; i++) {
        mem_map->buffer[i]=-1;
    }

    /* Inicialización de mutex y variables de condición */
    pthread_mutex_init(&mem_map->mutex, 0);
    pthread_mutex_init(&mem_map->mutex_even_sum, 0);
    pthread_mutex_init(&mem_map->mutex_odd_sum, 0);

    pthread_cond_init(&mem_map->cond_consumer, 0);

    pthread_cond_init(&mem_map->cond_producer, 0);

    /* Inicilizamos el vector T */
    for(i = 0; i < TAM; i++){
        mem_map->T[i] = (int) i/2; // El cast redondea a la baja
        sum_truth += mem_map->T[i];
    }

    printf("Suma verdadera: %d\n", sum_truth);

    /* Crear consumidores */
    for (j = 0; j < C; j++){

        if((pthread_create(&consumer_ids[j], NULL, consumer, (void*) j)) != 0){
            printf("Error creando el hilo consumidor %d\n", j);
        }
        //printf("\n Proceso CONSUMIDOR [%d] acabando...\n", getpid());
    }

    /* Crear productores */
    for (j = 0; j < P; j++){
        if((pthread_create(&producer_ids[j], NULL, producer, (void*) j)) != 0){
            printf("Error creando el hilo productor %d\n", j);
        }

        //printf("\n Proceso PRODUCTOR [%d] acabando...\n", getpid());
    }


    /* Esperamos a que acaben  de producir los productores. Cuando el i-ésimo productor termina de producir: mem_map->flag[i] == 1 */
    i=0;
    do{
        if(mem_map->flag[i] == 1) i++; /* Si el hilo i ha marcado la flag, pasamos a comprobar si el siguiente hilo productor ha terminado de producir */
    }while(mem_map->flag[i] == 0 || i != P-1);



    printf("Los productores han terminado de producir\n");

    /* Liberemos todos los consumidores que puedan estar bloqueados */
     pthread_cond_broadcast(&mem_map->cond_consumer);

    /* Esperamos a que acaben los productores */
    for (j=0; j<P; j++) {
        pthread_join(producer_ids[j], NULL);

    }

    /* Esperamos a que acaben los consumidores */
    for (j=0; j<C; j++) {
        pthread_join(consumer_ids[j], NULL);

    }

    /* Printeamos la suma calculada por el hilo principal y la sua calculada por los hilos productores y consumidores en colaboración */
    printf("Suma verdadera: %d |\tSuma calculada: %d\n", sum_truth, mem_map->even_sum + mem_map->odd_sum);

    /* Cierre de mutex y variables de condición */
    pthread_cond_destroy(&mem_map->cond_consumer);
    pthread_cond_destroy(&mem_map->cond_producer);
    pthread_mutex_destroy(&mem_map->mutex);

    /* Liberamos la proyección de memoria */
    free(mem_map);

    return 0;
}
