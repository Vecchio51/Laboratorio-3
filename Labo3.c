#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// Definición de la estructura compartida
typedef struct {
    sem_t semaforo;
    float saldo;
} RecursoCompartido;

// Prototipos de las funciones de los hijos
void credito(char *archivo_montos, int p[], RecursoCompartido *compartido);
void debito(char *archivo_montos, int p[], RecursoCompartido *compartido);



main()
{
    RecursoCompartido *compartido;
    int pipe_credito[2];
    int pipe_debito[2];
    pid_t pid_credito, pid_debito;

// 1. Crear la memoria compartida para el struct
    compartido = mmap(NULL, sizeof(RecursoCompartido), PROT_READ | PROT_WRITE, 
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (compartido == MAP_FAILED) {
        perror("Error en mmap");
        exit(EXIT_FAILURE);
    }
// Inicializar el saldo en 0 y el semáforo binario en 1
    compartido->saldo = 0.0;
    if (sem_init(&(compartido->semaforo), 1, 1) == -1) {
        perror("Error en sem_init");
        exit(EXIT_FAILURE);
    }
// 2. Crear los pipes para comunicación con los hijos



// 3. Crear el primer hijo: CRÉDITO



// ----------------------------------------------------
// CÓDIGO DEL PADRE
// ----------------------------------------------------
// El padre solo lee, así que cierra las escrituras de ambos pipes
    close(pipe_credito[1]);
    close(pipe_debito[1]);

    float monto_recibido;
    int fin_credito = 0, fin_debito = 0;

    printf("Padre leyendo las transacciones...\n\n");
   
    }

/* * en esta parte hay quye crear un Bucle de lectura usando select() o lectura no bloqueante de los pipes.
* Para cumplir la consigna de forma directa, leemos de a un pipe
* hasta que se vacíen (read devuelve 0 cuando el hijo cierra su extremo).
*/
    
// Leer del pipe de Crédito hasta que cierre


    return 0;
}