#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	// Quantum
	int q = atoi(argv[3]);

	// Variables para almacenar lo datos extraidos del input_file
	char name[10];
	int pid, t_start, t_cpu_burst, n_burst, io_wait, t_deadline;

	// Queues
	Queue* High = NULL;
	Queue* Low = NULL;

	// Lista para almacenar los procesos al crearlos
	int num_processes = input_file->len;
	Process** processes = malloc(num_processes * sizeof(Process));

	/*Mostramos el archivo de input en consola*/
	printf("Nombre archivo: %s\n", file_name);
	printf("Cantidad de procesos: %d\n", input_file->len);
	printf("Procesos:\n");

	for (int i = 0; i < input_file->len; ++i)
	{
		// Se extraen los datos del input
		strcpy(name, input_file->lines[i][0]);
		pid = atoi(input_file->lines[i][1]);
        t_start = atoi(input_file->lines[i][2]);
        t_cpu_burst = atoi(input_file->lines[i][3]);
        n_burst = atoi(input_file->lines[i][4]);
    	io_wait = atoi(input_file->lines[i][5]);
        t_deadline = atoi(input_file->lines[i][6]);
		Process* process = init_process( name, pid, t_start, t_cpu_burst, n_burst, io_wait, t_deadline);

		processes[i] = process;

		for (int j = 0; j < 7; ++j)
		{
			printf("%s ", input_file->lines[i][j]);
		}
		printf("\n");
	}

	input_file_destroy(input_file);

	// Round-Robin
	int current_time = 0;
	int completed_processes = 0;
	Process* CPU = NULL;

	while (completed_processes < num_processes) {
		//Actualiza los procesos que han terminado su tiempo de espera de I/O
		for (int i = 0; i < num_processes; i++) {
			if (strcmp(processes[i]->state, "WAITING") == 0) {
				if (processes[i]->current_io_wait == processes[i]->io_wait) {
                	strcpy(processes[i]->state, "READY");
					processes[i]->current_io_wait = 0;
            	} else {
					processes[i]->current_io_wait++;
				}
			}
        }

		// Actualiza estado si hay un proceso en estado RUNNING
		if (CPU) {
			if (CPU->current_t_burst == CPU->t_cpu_burst) {
				if (CPU->current_n_burst < CPU->n_burst) {
					strcpy(CPU->state, "WAITING");
					CPU->current_n_burst++;
					CPU->T_LCPU = current_time;
					CPU = NULL;
				} else {
					strcpy(CPU->state, " FINISHED");
					completed_processes++;
					CPU = NULL;
				}
			} else if (CPU->current_quantum == CPU->quantum) {
				strcpy(CPU->state, "READY");
				CPU->T_LCPU = current_time;
				CPU->current_quantum = 0;
				CPU->quantum = q;
				if (!CPU->in_low) {
					add_process(&Low, CPU);	
				}
				CPU = NULL;
			} else {
				CPU->current_quantum++;
				CPU->current_t_burst++;
			}
		}

        // Se agregan al queue lo procesos de acuerdo a su t_start
        for (int i = 0; i < num_processes; i++) {
            if (processes[i]->t_start == current_time) {
                add_process(&High, processes[i]);
				processes[i]->quantum = 2 * q;
            }
        }

		// Revisa si se cumple la condicion para subir a la cola High
		Queue* current = Low;
    	while (current != NULL) {
			if (2*current->process->t_deadline < current_time - current->process->T_LCPU) {
				Process* moving_process= extract_process(&Low, current->process->pid);
				add_process(&High, moving_process);
			}
        	current = current->next;
    	}

		// Ingresa el proceso de mayor prioridad en estado READY a la CPU

		current_time++;
	}
}