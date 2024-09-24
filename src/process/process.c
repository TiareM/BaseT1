#include "process.h"

Process* init_process(char* name, int pid, int t_start, int t_cpu_burst, int n_burst, int io_wait, int t_deadline) {
    Process* process = malloc(sizeof(Process));

    strcpy(process->name, name);
    process->pid = pid;
    strcpy(process->state, "READY");
    process->t_cpu_burst = t_cpu_burst;
    process->n_burst = n_burst;
    process->io_wait = io_wait;
    process->t_deadline = t_deadline;
    process->t_start = t_start;
    process->current_io_wait = 0;
    process->current_t_burst = 0;
    process->current_quantum = 0;
    process->current_n_burst = 0;
    process->in_low = false;
    process->T_LCPU = 0;

    return process;
}
