#include "producer.h"


int produce_item(){

    return(get_rand());
}

void insert_item(mem_shared *mem_map, int item){
    int count;
    int buffer[N];
    struct timeval tv;

    /* Inicializamos la semilla */
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    /* Obtemos el número de elementos en el buffer */
    sem_getvalue(mem_map->full, &count);

    /* Insertamos en la posición correspondiente */
    mem_map->buffer[count] = item;


    /* Printeamos el buffer para constatar posibles carreras críticas */
    printf("Buffer(PRODUCTOR : %d): | %d | %d | %d | %d | %d | %d | %d | %d |\n", getpid(), mem_map->buffer[0], mem_map->buffer[1], mem_map->buffer[2], mem_map->buffer[3],
           mem_map->buffer[4], mem_map->buffer[5],mem_map->buffer[6], mem_map->buffer[7]);

}


void producer(mem_shared *mem_map){
    int item; /* Elemento a extraer del buffer */
    int i=0; /* Contador del bucle*/


    while(i<BUC){

        /* Metemos un sleep fuera de la región crítica */

        sleep(rand()%3);
        item = produce_item(); /* Producimos un entero aleatorio entre 0 y 10 */
        sem_wait(mem_map->empty); /* down al semáforo empty*/
        sem_wait(mem_map->mutex); /* down al semáforo mutex*/
        insert_item(mem_map, item); /* insertamos el item en el buffer */
        //jfnsakjljafn
        sem_post(mem_map->mutex); /* up al semáforo mutex*/
        sem_post(mem_map->full); /* up al semáforo full*/


        i++; /* Aumentamos el contador */
    }
}
