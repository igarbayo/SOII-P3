
#include "consumer.h"

extern mem_shared * mem_map;


void print_bufferc(int id);


void contribute_consumer(){
    int i, lim;

    /* Limite del bucle */
    lim = mem_map->index_even_sum + ((rand()%3)+2);
    i=mem_map->index_even_sum;

    for(i; i < lim && i < TAM; i += 2){
        mem_map->even_sum += mem_map->T[i];
    }
    /* Actualizamos el índice de indexado */
    mem_map->index_even_sum = i;
}

int remove_item(int* sum, int id){
    int i, item;

    item = mem_map->buffer[mem_map->count-1];

    *sum = item;
    for(i=0; i<mem_map->count-1; i++) *sum += mem_map->buffer[i]; // Sumamos todos menos el que hemos eliminado
 
    mem_map->buffer[mem_map->count-1] = -1; // Marcamos como eliminado el elemento
    mem_map->count--;

    /* Printeamos el buffer para constatar posibles carreras críticas */
    print_bufferc(id);

    return item;
}

void consume_item(int item, int sum, int id){
    printf("(%d) Item extraído:\t %d\t Suma: %d\n",getpid(), item, sum);
}


void* consumer(void* args){
    int i=0;
    int sum=0;
    int item=0;
    int theoretical_even_sum;

    /* Recogemos los argumentos*/
    int id = (int)args;




    while(i < BUC_CONS){

        /* Metemos un sleep fuera de la región crítica */
        usleep(rand()%3);
        pthread_mutex_lock(&mem_map->mutex);         // Obtiene acceso exclusivo al buffer
        while(mem_map->count == 0) pthread_cond_wait(&mem_map->cond_consumer, &mem_map->mutex); //Si el buffer esté vacío, bloquéate. NOTA: PONER POR QUÉ SE PSA MUTEX
        item = remove_item(&sum, id);         // Elimina un elemento

        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_even_sum)) == 0){
            contribute_consumer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_even_sum); /* Cuando termines de contribuir sal de la región crítica */
        }
        pthread_cond_signal(&mem_map->cond_producer);    // Despierta al productor
        pthread_mutex_unlock(&mem_map->mutex);       // Libera el acceso al buffer
        consume_item(item, sum, id);

        i++;
    }



    do{
        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_even_sum)) == 0){
            contribute_consumer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_even_sum);

        }

        // printf("Suma pares: %d \t %d\n", mem_map->even_sum, mem_map->theoretical_even_sum);
    }while(mem_map->index_even_sum < TAM); /* Mientras no se haya terminado la suma de los pares */


}


void print_bufferc (int id) {
    printf("Buffer (CONSUMIDOR = %d):\t ", id);
    for (int i=0; i<N; i++) {
        (mem_map->buffer[i]==-1) ? printf("| - ") : printf("| %d ", mem_map->buffer[i]);
    }
    printf("|\tSuma pares   = %d\n", mem_map->even_sum);
}
