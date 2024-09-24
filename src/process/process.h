#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct process {
    char name[50];
    int pid;
    char state[10];           // {RUNNING, READY, WAITING, FINISHED}
    int t_cpu_burst;          // Tiempo de ejecucion por rafaga  
    int current_t_burst;      // Tiempo de espera actual de rafaga
    int n_burst;              // Numero de rafagas de ejecucion en CPU
    int current_n_burst;      // Numero de burst completados
    int io_wait;              // Tiempo de espera para I/O entre rafagas
    int current_io_wait;      // Tiempo de espera actual para I/O
    int t_deadline;           // Deadline de ejecucion
    int t_start;              // Tiempo en el que el proceso entra a la cola por primera vez
    int quantum;              // Quantum de la cola en que esta
    int current_quantum;      // Tiempo ejecutado del quantum
    bool in_low;              // Si esta en la cola Low
    int T_LCPU;               // El tiempo (tick) en que el proceso salio por ultima vez de la CPU
} Process;

Process* init_process(char* name, int pid, int t_start, int t_cpu_burst, int n_burst, int io_wait, int t_deadline);
