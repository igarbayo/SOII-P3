#include "consumer.h"


int remove_item(mem_shared* mem_map, int* sum){
    int i, item, count;

    /* Calculamos count a partir de empty, que está actualizado, en vez de full*/
    sem_getvalue(mem_map->empty, &count);
    count = (N-1)-count;


    item = mem_map->buffer[count];
    // mem_map->buffer[mem_map->count]=-17; //Indicamos valor no válido
    //item = mem_map[mem_map[0] + 1];

    *sum = item;
    for(i=0; i<count; i++) *sum += mem_map->buffer[i]; // Sumamos todos menos el que hemos eliminado
 
    mem_map->buffer[count] = -1;

    printf("Buffer(CONSUMIDOR : %d): | %d | %d | %d | %d | %d | %d | %d | %d |\n", getpid(), mem_map->buffer[0], mem_map->buffer[1], mem_map->buffer[2], mem_map->buffer[3],
           mem_map->buffer[4], mem_map->buffer[5],mem_map->buffer[6], mem_map->buffer[7]);

    return item;
}

void consume_item(int item, int sum){
    printf("(%d) Item extraído:\t %d\t Suma: %d\n",getpid(), item, sum);
}


void consumer(mem_shared *mem_map){
    int i=0;
    int item, sum=0;

    while(i<BUC){

        /* Metemos un sleep fuera de la región crítica */
        sleep(rand()%3);
        sem_wait(mem_map->full);
        sem_wait(mem_map->mutex);
        item = remove_item(mem_map, &sum);
        sem_post(mem_map->mutex);
        sem_post(mem_map->empty);
        consume_item(item, sum);

        i++;
    }
}
