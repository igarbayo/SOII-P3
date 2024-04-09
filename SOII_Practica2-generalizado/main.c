#include "consumer.h"
#include "producer.h"
#include <sys/mman.h>


#define N 8


int main(int argc, char** argv) {
    mem_shared * mem_map;
    pid_t child;
    int n_processes, j;


    if (argc != 2){
            printf("Número de argumentos incorrecto: %s <número de procesos>", argv[0]);
            exit(1);
    }

    /* Unlink de los semáforos */

    sem_unlink("empty");
    sem_unlink("mutex");
    sem_unlink("full");

    n_processes = atoi(argv[1]); 


    if((mem_map = (mem_shared*)mmap(NULL, sizeof(mem_shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED){/* Proyectamos en memoria el archivo a escribir */
        fprintf(stderr, "Error al hacer mmap\n");
        exit(EXIT_FAILURE);
    }



    /* Inicializamos los semáforos */
    mem_map->empty = sem_open("empty", O_CREAT, 0700, N);
    mem_map->mutex = sem_open("mutex", O_CREAT, 0700, 1);
    mem_map->full = sem_open("full", O_CREAT, 0700, 0);


    /* Crear consumidores */
    for (j = 0; j < n_processes; j++){
        if(fork() == 0){

            mem_map->empty = sem_open("empty",0);
            mem_map->mutex = sem_open("mutex",0);
            mem_map->full = sem_open("full",0);

            consumer(mem_map);


            printf("\n Proceso CONSUMIDOR [%d] acabando...\n", getpid());

            /* Cerramos los semáforos */
            sem_close(mem_map->empty);
            sem_close(mem_map->mutex);
            sem_close(mem_map->full);

            /* Liberamos la proyección de memoria */
            if(munmap(mem_map, sizeof(mem_shared))) printf("Error liberando memoria compartida\n");

            /* Unlink de los semáforos */
            sem_unlink("empty");
            sem_unlink("mutex");
            sem_unlink("full");
            exit(EXIT_SUCCESS);
        }
    }

    /* Crear productores */
    for (j = 0; j < n_processes; j++){
        if(fork() == 0){

            mem_map->empty = sem_open("empty",0);
            mem_map->mutex = sem_open("mutex",0);
            mem_map->full = sem_open("full",0);

            producer(mem_map);

            printf("\n Proceso PRODUCTOR [%d] acabando...\n", getpid());

            /* Cerramos los semáforos */
            sem_close(mem_map->empty);
            sem_close(mem_map->mutex);
            sem_close(mem_map->full);

            /* Liberamos la proyección de memoria */
            if(munmap(mem_map, sizeof(mem_shared))) printf("Error liberando memoria compartida\n");

            /* Unlink de los semáforos */
            sem_unlink("empty");
            sem_unlink("mutex");
            sem_unlink("full");
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
