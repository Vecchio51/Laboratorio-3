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
    pipe(pipe_credito);
    //pipe(pipe_debito);


// 3. Crear el primer hijo: CRÉDITO

    pid_credito = fork();

    if(pid_credito == 0)
    {
        close(pipe_credito[0]);
        close(pipe_debito[0]);
        close(pipe_debito[1]);
        credito("credito.txt", pipe_credito, compartido);
        exit(0);
    }

    pid_debito = fork();

    if(pid_debito == 0)
    {
        close(pipe_debito[0]);
        close(pipe_credito[0]);
        close(pipe_credito[1]);
        debito("debito.txt", pipe_debito, compartido);
        exit(0);
    }
// ----------------------------------------------------
// CÓDIGO DEL PADRE
// ----------------------------------------------------
// El padre solo lee, así que cierra las escrituras de ambos pipes

    printf("Padre leyendo las transacciones...\n\n");

    close(pipe_credito[1]);
    close(pipe_debito[1]);

    float monto_recibido;
    ssize_t bytes;
    int fin_credito = 0, fin_debito = 0;
    while (!fin_credito || !fin_debito){
        if(!fin_credito){
            bytes = read(pipe_credito[0], &monto_recibido, sizeof(float));
            if (bytes == 0)
                fin_credito = 1;
            else 
                printf("Credito: +%.2f\n", monto_recibido);
        }
        if (!fin_debito){
            bytes = read(pipe_debito[0], &monto_recibido, sizeof(float));
            if(bytes == 0)
                fin_debito = 1;
            else   
                printf("Debito: -%.2f\n", monto_recibido);
        }
    }
    wait(NULL);  //Coloco dos wait(NULL) ya que tenemos dos hijos, entonces necesito llamarlo dos veces (uno por hijo)
    wait(NULL);

   
    

/* * en esta parte hay quye crear un Bucle de lectura usando select() o lectura no bloqueante de los pipes.
* Para cumplir la consigna de forma directa, leemos de a un pipe
* hasta que se vacíen (read devuelve 0 cuando el hijo cierra su extremo).
*/
    
// Leer del pipe de Crédito hasta que cierre


    return 0;
}

void credito (char *archivo_montos, int p[], RecursoCompartido *compartido){
    FILE *f = fopen(archivo_montos, "r");
    if (f == NULL){
        perror("Error abriendo archivo credito");
        exit(EXIT_FAILURE);
    }

    float monto;

    while (fcanf(f, "%f", &monto) == 1){
        sem_wait(&(compartido->semaforo));  //si otro proceso esta dentro, aca me bloqueo hasta que salga

        compartido->saldo += monto;         //solo un proceso a la vez llega aca

        sem_post(&(compartido->semaforo));  //liberar el semaforo 

        write(p[1], &monto, sizeof(float)); //enviar el monto al padre por el pipe
    }
    fclose(f);
    close(p[1]);                            //esto hace que el padre reciba 0 bytes -> sabe que este hijo termino
}

void debito (char *archivo_montos, int p[], RecursoCompartido *compartido){
    FILE *f = fopen(archivo_montos, "r");
    if (f == NULL){
        perror("Error abriendo archivo debito");
        exit(EXIT_FAILURE);
    }
    float monto;

    while (fscanf(f, "%f", &monto) == 1){
        sem_wait(&(compartido->semaforo));

        compartido->saldo -= monto;

        sem_post(&(compartido->semaforo));

        write(p[1], &monto, sizeof(float));
    }
    fclose(f);
    close(p[1]);
    
}
