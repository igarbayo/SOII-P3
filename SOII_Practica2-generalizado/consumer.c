
#include "consumer.h"

/* Declaración de estructrua compartida como externa para que sea visible por las funciones */
extern mem_shared * mem_map;
/* Array para los valores de sleep */
extern int sleepValues[6];
/* Contador de iteraciones en las que no se trabaja */
extern int trabajoC;

/**
 * Imprime el buffer en el formato de un consumidor
 * @param id entero con el id único del consumidor
 */
void print_bufferc(int id);

/**
 * Función para que un consumidor intente contribuir en la suma de los elementos impares
 */
void contribute_consumer(){
    int i, lim;

    /* Limite del bucle */
    lim = mem_map->index_odd_sum + ((rand()%3)+2);
    
    /* Obtenemos posición para sumar los impares */
    i=mem_map->index_odd_sum;

    /* Sleep con argumento 6 */
    sleep(sleepValues[5]);

    /* Se contribuye a la suma */
    for(i; i < lim && i < TAM; i += 2){
        mem_map->odd_sum += mem_map->T[i];
    }
    /* Actualizamos el índice de indexado */
    mem_map->index_odd_sum = i;
}

/**
 * Función para eliminar un elemento
 * @param sum variable para almacenar la suma de los elementos del buffer
 * @param id identificador único del consumidor
 * @return elemento eliminado
 */
int remove_item(int* sum, int id){
    int i, item;

    /* Sleep con argumento 4 */
    sleep(sleepValues[3]);

    /* Extraemos un elemento y decrementamos el contador */
    item = mem_map->buffer[mem_map->count-1];

    *sum = item;
    for(i=0; i<mem_map->count-1; i++) *sum += mem_map->buffer[i]; /* Sumamos todos menos el que hemos eliminado */
 
    mem_map->buffer[mem_map->count-1] = -1; /* Marcamos como eliminado el elemento */
    mem_map->count--; /* Actualizamos el contador */

    /* Printeamos el buffer para constatar posibles carreras críticas */
    print_bufferc(id);

    return item;
}

/**
 * Función para consumir un elemento
 * @param item elemento a consumir
 * @param sum variable para acumular la suma de los elementos del buffer
 * @param id identificador único del consumidor
 */
void consume_item(int item, int sum, int id){
    /* Sleep con argumento 5 */
    sleep(sleepValues[4]);
    printf("(%d) Item extraído:\t %d\t Suma: %d\n",id, item, sum);
}

/**
 * Función de comportamiento de un consumidor
 * @param args argumentos a pasar con el formato para un hilo
 */
void* consumer(void* args){
    int i=0;
    int sum=0;
    int item;

    /* Recogemos los argumentos*/
    int id = (int)args;

    while(i < BUC_CONS){

        /* Metemos un sleep fuera de la región crítica */
        usleep(rand()%3);
        pthread_mutex_lock(&mem_map->mutex); /* Obtiene acceso exclusivo al buffer */
        while(mem_map->count == 0) { /* Mientras el buffer esté vacío, bloquéate */
            pthread_cond_wait(&mem_map->cond_consumer, &mem_map->mutex);
            /* Incrementa la variable que mide las iteraciones en las que los consumidores no trabajan, por estar bloqueados */
            trabajoC++;
        }
        item = remove_item(&sum, id); /* Elimina un elemento */

        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_odd_sum)) == 0){
            contribute_consumer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_odd_sum); /* Cuando termines de contribuir sal de la región crítica */
        }
        pthread_cond_signal(&mem_map->cond_producer); /* Despierta al productor */
        pthread_mutex_unlock(&mem_map->mutex); /* Libera el acceso al buffer */
        consume_item(item, sum, id);

        i++; /* Aumentamos el contador */
    }

    do{
        /* Si no hay otro hilo en la región crítica, entra */
        if((pthread_mutex_trylock(&mem_map->mutex_odd_sum)) == 0){
            contribute_consumer();
            /* Cuando termines de contribuir sal de la región crítica */
            pthread_mutex_unlock(&mem_map->mutex_odd_sum);

        }
    }while(mem_map->index_odd_sum < TAM); /* Mientras no se haya terminado la suma de los impares */

    printf("Soy el Consumidor %d y he acabado de contribuir. Valor suma impares:\t%d\n", id, mem_map->odd_sum);

}

void print_bufferc (int id) {
    /* Imprime el id del consumidor */
    printf("Buffer (CONSUMIDOR = %d):\t ", id);
    /* Impresión de los elementos del buffer con el formato | X | Y | Z | - | - | */
    for (int i=0; i<N; i++) {
        (mem_map->buffer[i]==-1) ? printf("| - ") : printf("| %d ", mem_map->buffer[i]);
    }
    /* Muestra la suma temporal de los impares */
    printf("|\tSuma impares   = %d\n", mem_map->even_sum);
}
