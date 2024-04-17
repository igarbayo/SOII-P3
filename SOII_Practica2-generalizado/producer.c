#include "producer.h"

extern mem_shared * mem_map;

void print_bufferp (int id);

void contribute_producer(){
    int i, lim;

    /* Limite del bucle */
    lim = mem_map->index_odd_sum + ((rand()%3)+2);
    i=mem_map->index_odd_sum;

    for(i; i < lim && i < TAM; i += 2){
        mem_map->odd_sum += mem_map->T[i];
    }
    /* Actualizamos el índice de indexado */
    mem_map->index_odd_sum = i;
}

int produce_item(){
    return(get_rand());
}

void insert_item(int item, int id) {
    struct timeval tv;

    /* Inicializamos la semilla */
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    /* Insertamos en la posición correspondiente */
    mem_map->buffer[mem_map->count] = item;

    /* Actualizamos count */
    mem_map->count++;

    /* Printeamos el buffer para constatar posibles carreras críticas */
    print_bufferp(id);
}


void* producer(void* args){
    int item; /* Elemento a extraer del buffer */
    int i=0; /* Contador del bucle*/

    int id = (int)args;

    while(i<BUC_PROD){

        /* Metemos un sleep fuera de la región crítica */
        usleep(rand()%3);

        item = produce_item(); /* Producimos un entero aleatorio entre 0 y 10 */
        pthread_mutex_lock(&mem_map->mutex);
        while(mem_map->count == N) pthread_cond_wait(&mem_map->cond_producer, &mem_map->mutex); // Si esté lleno.
        insert_item(item, id); /* insertamos el item en el buffer */

        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_odd_sum)) == 0){
            contribute_producer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_odd_sum); /* Cuando termines de contribuir sal de la región crítica */
        }

        pthread_cond_signal(&mem_map->cond_consumer);
        pthread_mutex_unlock(&mem_map->mutex);

        i++; /* Aumentamos el contador */
    }

    mem_map->flag[id] = 1; /* Indicamos a través de la flag que el hilo i-ésimo a terminado de producir */


    do{
        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_odd_sum)) == 0){
            contribute_producer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_odd_sum); /* Cuando termines de contribuir sal de la región crítica */
        }
    }while(mem_map->index_odd_sum < TAM);

}


void print_bufferp (int id) {
    printf("Buffer  (PRODUCTOR = %d):\t ", id);
    for (int i=0; i<N; i++) {
        (mem_map->buffer[i]==-1) ? printf("| - ") : printf("| %d ", mem_map->buffer[i]);
    }
    printf("|\tSuma impares = %d\n", mem_map->odd_sum);
}
