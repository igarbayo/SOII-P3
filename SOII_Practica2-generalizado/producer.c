#include "producer.h"

extern mem_shared * mem_map;
extern int sleepValues[6];
extern int trabajoP;

void print_bufferp (int id);

void contribute_producer(){
    int i, lim;

    /* Limite del bucle */
    lim = mem_map->index_even_sum + ((rand()%3)+2);
    i=mem_map->index_even_sum;

    usleep(sleepValues[2]);

    for(; i < lim && i < TAM; i += 2){
        mem_map->even_sum += mem_map->T[i];
    }
    /* Actualizamos el índice de indexado */
    mem_map->index_even_sum = i;
}

int produce_item(){
    usleep(sleepValues[0]);
    return(get_rand());
}

void insert_item(int item, int id) {
    struct timeval tv;

    /* Inicializamos la semilla */
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    usleep(sleepValues[1]);

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
        pthread_mutex_lock(&mem_map->mutex); /* Bloqueamos el mutex */
        while(mem_map->count == N) { /* Mientras esté lleno */
            pthread_cond_wait(&mem_map->cond_producer, &mem_map->mutex);
            trabajoP++;
        }
        insert_item(item, id); /* Insertamos el item en el buffer */

        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_even_sum)) == 0){
            contribute_producer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_even_sum);
        }

        pthread_cond_signal(&mem_map->cond_consumer); /* Desbloqueamos a algún consumidor bloqueado */
        pthread_mutex_unlock(&mem_map->mutex); /* Liberamos el mutex */

        i++; /* Aumentamos el contador */
    }

    mem_map->flag[id] = 1; /* Indicamos a través de la flag que el hilo i-ésimo a terminado de producir */

    do{
        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_even_sum)) == 0){
            contribute_producer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_even_sum); /* Cuando termines de contribuir sal de la región crítica */
        }
    }while(mem_map->index_even_sum < TAM);

    printf("Soy el Productor %d y he acabado de contribuir. Valor suma pares:\t%d\n", id, mem_map->even_sum);

}


void print_bufferp (int id) {
    printf("Buffer  (PRODUCTOR = %d):\t ", id);
    for (int i=0; i<N; i++) {
        (mem_map->buffer[i]==-1) ? printf("| - ") : printf("| %d ", mem_map->buffer[i]);
    }
    printf("|\tSuma pares = %d\n", mem_map->odd_sum);
}
