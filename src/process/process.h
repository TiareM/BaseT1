#pragma once

typedef struct process {
    char name[50];
    int pid;
    char state[10];           // {RUNNING, READY, WAITING, FINISHED}
    int t_cpu_burst;          // Tiempo de ejecucion por rafaga  
    int n_burst;              // Numero de rafagas de ejecucion en CPU
    int io_wait;              // Tiempo de espera para I/O entre rafagas
    int t_deadline;           // Deadline de ejecucion
    int t_start;              // Tiempo en el que el proceso entra a la cola por primera vez
} Process;

Process* init_process(char* name, int pid, int t_start, int t_cpu_burst, int n_burst, int io_wait, int t_deadline);
