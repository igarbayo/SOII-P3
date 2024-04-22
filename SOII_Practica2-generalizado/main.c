#include "consumer.h"
#include "producer.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>


mem_shared * mem_map; // Memoria compartida con el mutex y los cond_t

int sleepValues[6];   // Array con los valores necesarios para forzar el comportamiento deseado mediante llamadas a sleep()
int trabajoP;         // Cuenta el número de veces que los productores se quedan esperando (iteraciones no trabajadas)
int trabajoC;         // Cuenta el número de veces que los consumidores se quedan esperando (iteraciones no trabajadas)


int main(int argc, char** argv) {
    pthread_t consumer_ids[C]; // Array con los ids de los consumidores
    pthread_t producer_ids[P]; // Array con los ids de los productores
    int i, j, k;
    int sum_truth=0;

    trabajoP=0;
    trabajoC=0;

    /* Si no se pasan 6 valores como argumentos */
    if (argc != 7) {
        fprintf(stderr, "Error: debe incluir 6 argumentos para los sleep:\n"
                       "    · Producción item\n"                            // argv[1] = sleepValues[0]
                       "    · Inclusión item en buffer\n"                   // argv[2] = sleepValues[1]
                       "    · Contribución a suma de T (productor)\n"       // argv[3] = sleepValues[2]
                       "    · Extracción item de buffer\n"                  // argv[4] = sleepValues[3]
                       "    · Consumo de item\n"                            // argv[5] = sleepValues[4]
                       "    · Contribución a suma de T (consumidor)\n");    // argv[6] = sleepValues[5]
        exit(EXIT_FAILURE);
    }

    /* Asignamos los argumentos a sleepValues */
    for (k=0; k<6; k++) {
        sleepValues[k]=atoi(argv[k+1]);
    }

    /* Inicializamos la estructura con el buffer */
    mem_map = (mem_shared*) malloc (sizeof(mem_shared));

    /* Cuenta a 0 */
    mem_map->count=0;

    /* Para que sume impares = odd */
    mem_map->index_odd_sum = 1;

    /* Para que sume pares = even */
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
    }

    /* Crear productores */
    for (j = 0; j < P; j++){
        if((pthread_create(&producer_ids[j], NULL, producer, (void*) j)) != 0){
            printf("Error creando el hilo productor %d\n", j);
        }
    }

    /* Esperamos a que acaben  de producir los productores. Cuando el i-ésimo productor termina de producir: mem_map->flag[i] == 1 */
    for (i=0; i<P; i++) {
        do{
            if(mem_map->flag[i] == 1) break; /* Si el hilo i ha marcado la flag, pasamos a comprobar si el siguiente hilo productor ha terminado de producir */
        }while(mem_map->flag[i] == 0);
    }

    printf("Los productores han terminado de producir\n");

    /* Liberemos todos los consumidores que puedan estar bloqueados */
     pthread_cond_broadcast(&mem_map->cond_consumer);

    /* Esperamos a que acaben los productores */
    for (j=0; j<P; j++) {
        pthread_join(producer_ids[j], NULL);
        printf("Ha acabado P%d\n", j);

    }

    /* Esperamos a que acaben los consumidores */
    for (j=0; j<C; j++) {
        pthread_join(consumer_ids[j], NULL);
        printf("Ha acabado C%d\n", j);
    }

    /* Printeamos la suma calculada por el hilo principal y la sua calculada por los hilos productores y consumidores en colaboración */
    printf("Suma verdadera: %d |\tSuma calculada: %d\n", sum_truth, mem_map->even_sum + mem_map->odd_sum);
    printf("trabajoP = %d\ttrabajoC = %d\n", trabajoP, trabajoC);

    (trabajoP>trabajoC) ? printf("Han trabajado más los consumidores\n") : printf("Han trabajado más los productores\n");

    /* Cierre de mutex y variables de condición */
    pthread_cond_destroy(&mem_map->cond_consumer);
    pthread_cond_destroy(&mem_map->cond_producer);
    pthread_mutex_destroy(&mem_map->mutex);

    /* Liberamos la proyección de memoria */
    free(mem_map);

    return 0;
}
