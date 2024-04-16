
#include "consumer.h"

extern mem_shared * mem_map;


int remove_item(int* sum){
    int i, item;

    item = mem_map->buffer[mem_map->count-1];

    *sum = item;
    for(i=0; i<mem_map->count-1; i++) *sum += mem_map->buffer[i]; // Sumamos todos menos el que hemos eliminado
 
    mem_map->buffer[mem_map->count-1] = -1; // Marcamos como eliminado el elemento
    mem_map->count--;

    printf("Buffer(CONSUMIDOR : %d):\t| %d | %d | %d | %d | %d | %d | %d | %d |\n", (int)pthread_self(), mem_map->buffer[0], mem_map->buffer[1], mem_map->buffer[2], mem_map->buffer[3],
           mem_map->buffer[4], mem_map->buffer[5],mem_map->buffer[6], mem_map->buffer[7]);

    return item;
}

void consume_item(int item, int sum){
    printf("(%d) Item extraído:\t %d\t Suma: %d\n",getpid(), item, sum);
}


void* consumer(){
    int i=0;
    int sum=0;
    int item=0;

    while(i<BUC){

        /* Metemos un sleep fuera de la región crítica */
        sleep(rand()%3);
        pthread_mutex_lock(&mem_map->mutex);         // Obtiene acceso exclusivo al buffer
        while(mem_map->count ==0) pthread_cond_wait(&mem_map->cond_consumer, &mem_map->mutex); //Mientras el buffer esté vacío, bloquéate. NOTA: PONER POR QUÉ SE PSA MUTEX
        item = remove_item(&sum);         // Elimina un elemento
        pthread_cond_signal(&mem_map->cond_producer);    // Despierta al productor
        pthread_mutex_unlock(&mem_map->mutex);       // Libera el acceso al buffer
        consume_item(item, sum);

        i++;
    }
}
