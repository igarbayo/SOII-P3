#include "producer.h"

/* Declaración como externa de la estructura de memoria compartida para que sea visible por las funciones */
extern mem_shared * mem_map;
/* Array con los valores de los sleep */
extern int sleepValues[6];
/* Variable que cuenta el número de iteraciones no trabajadas de los productores */
extern int trabajoP;

/**
 * Función que imprime el buffer en formato del productor
 * @param id identificador único del productor
 */
void print_bufferp (int id);

/**
 * Función para intentar contribuir en la suma de los elementos pares de T
 */
void contribute_producer(){
    int i, lim;

    /* Limite del bucle */
    lim = mem_map->index_even_sum + ((rand()%3)+2);
    /* Obtener el valor de la siguiente posición a sumar */
    i=mem_map->index_even_sum;

    /* Sleep con argumento 3 */
    sleep(sleepValues[2]);

    /* Se contribuye a la sumna */
    for(; i < lim && i < TAM; i += 2){
        mem_map->even_sum += mem_map->T[i];
    }
    /* Actualizamos el índice de indexado */
    mem_map->index_even_sum = i;
}

/**
 * Función para producir un elemento
 * @return elemento a insertar en el buffer
 */
int produce_item(){
    /* Sleep con argumento 1 */
    sleep(sleepValues[0]);
    return(get_rand());
}

/**
 * Función para insertar un elemento en el buffer
 * @param item elemento a insertar
 * @param id identificador único del productor
 */
void insert_item(int item, int id) {
    struct timeval tv;

    /* Inicializamos la semilla */
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    /* Sleep con argumento 2 */
    sleep(sleepValues[1]);

    /* Insertamos en la posición correspondiente */
    mem_map->buffer[mem_map->count] = item;

    /* Actualizamos count */
    mem_map->count++;

    /* Printeamos el buffer para constatar posibles carreras críticas */
    print_bufferp(id);
}

/** 
 * Función de comportamiento para un productor
 * @param args argumentos para la función con formato de hilo
 */
void* producer(void* args){
    int item; /* Elemento a extraer del buffer */
    int i=0; /* Contador del bucle*/

    /* Obtenemos el id único */
    int id = (int)args;

    while(i<BUC_PROD){

        /* Metemos un sleep fuera de la región crítica */
        usleep(rand()%3);

        item = produce_item(); /* Producimos un entero aleatorio entre 0 y 10 */
        pthread_mutex_lock(&mem_map->mutex); /* Bloqueamos el mutex */
        while(mem_map->count == N) { /* Mientras esté lleno */
            pthread_cond_wait(&mem_map->cond_producer, &mem_map->mutex);
            /* Se incrementa la variable que cuenta las iteraciones no trabajadas para los productores */
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
    /* Imprimimos el id único del productor */
    printf("Buffer  (PRODUCTOR = %d):\t ", id);
    /* Imprimimos el buffer con el formato | X | Y | Z | - | - | */
    for (int i=0; i<N; i++) {
        (mem_map->buffer[i]==-1) ? printf("| - ") : printf("| %d ", mem_map->buffer[i]);
    }
    /* Mostramos el valor de la suma temporal de los elementos pares de T */
    printf("|\tSuma pares = %d\n", mem_map->odd_sum);
}
