#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc.
#include <string.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

// Function declarations
Process* select_highest_priority_process(Queue* queue, int Tactual);
void print_results(FILE *output_file, Process **processes, int num_processes, int current_time);

int main(int argc, char const *argv[])
{
	// archivo de lectura
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	// Archivo output
	char *output_file_name = (char *)argv[2];
	FILE *output_file = fopen(output_file_name, "w");

	// Quantum
	int q = atoi(argv[3]);

	// Variables para almacenar los datos extraidos del input_file
	char name[10];
	int pid, t_start, t_cpu_burst, n_burst, io_wait, t_deadline;

	// Queues
	Queue* High = NULL;
	Queue* Low = NULL;

	// Lista para almacenar los procesos al crearlos
	int num_processes = input_file->len;
	Process** processes = malloc(num_processes * sizeof(Process*));

	/* Mostramos el archivo de input en consola */
	printf("Nombre archivo: %s\n", file_name);
	printf("Cantidad de procesos: %d\n", input_file->len);
	printf("Procesos:\n");

	for (int i = 0; i < num_processes; ++i) {
		// Se extraen los datos del input
		strcpy(name, input_file->lines[i][0]);
		pid = atoi(input_file->lines[i][1]);
		t_start = atoi(input_file->lines[i][2]);
		t_cpu_burst = atoi(input_file->lines[i][3]);
		n_burst = atoi(input_file->lines[i][4]);
		io_wait = atoi(input_file->lines[i][5]);
		t_deadline = atoi(input_file->lines[i][6]);
		Process* process = init_process(name, pid, t_start, t_cpu_burst, n_burst, io_wait, t_deadline);

		processes[i] = process;

		for (int j = 0; j < 7; ++j) {
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

		// printf("--------- Tiempo: %d ---------\n", current_time);

		// Actualiza los procesos que han terminado su tiempo de espera de I/O
		for (int i = 0; i < num_processes; i++) {
			if (strcmp(processes[i]->state, "WAITING") == 0) {
				if (processes[i]->current_io_wait == processes[i]->io_wait) {
					strcpy(processes[i]->state, "READY");
					processes[i]->current_io_wait = 0;
					processes[i]->waiting_time++;
				} else {
					processes[i]->current_io_wait++;
				}
				processes[i]->waiting_time++;
			}
		}

		
        // Se agregan al queue los procesos de acuerdo a su t_start
        for (int i = 0; i < num_processes; i++) {
            if (processes[i]->t_start == current_time) {
                add_process(&High, processes[i]);
				processes[i]->quantum = 2 * q;
            }
        }

		// Revisa si se cumple la condicion para subir a la cola High
		Queue* current = Low;
    	while (current != NULL) {
			if (2 * current->process->t_deadline < current_time - current->process->T_LCPU) {
				Process* moving_process = extract_process(&Low, current->process->pid);
				add_process(&High, moving_process);
			}
        	current = current->next;
    	}

		// Actualiza estado si hay un proceso en estado RUNNING
		if (CPU != NULL) {
			// printf("Proceso: %s\n", CPU->name);

			if (CPU->current_quantum != CPU->quantum && CPU->current_t_burst != CPU->t_cpu_burst) {
				CPU->current_quantum++;
				CPU->current_t_burst++;
			}

			// printf("Quantum actual: %d\n", CPU->current_quantum);
			// printf("Burst actual: %d\n", CPU->current_t_burst);

			if (CPU->current_t_burst == CPU->t_cpu_burst) {
				if (CPU->current_n_burst < CPU->n_burst) {
					strcpy(CPU->state, "WAITING");
					CPU->current_n_burst++;
					CPU->T_LCPU = current_time;
					CPU = NULL;
				} else {
					strcpy(CPU->state, "FINISHED");
					CPU->turnaround_time = current_time - CPU->t_start;
					completed_processes++;
					if (CPU->in_low){
						extract_process(&Low, CPU->pid);
					}
					else {
						extract_process(&High, CPU->pid);
					}
					CPU = NULL;
				}
			} else if (CPU->current_quantum == CPU->quantum) {
				strcpy(CPU->state, "READY");
				CPU->T_LCPU = current_time;
				CPU->current_quantum = 0;
				CPU->quantum = q;
				CPU->num_interruptions++;
				if (!CPU->in_low) {
					add_process(&Low, CPU);
				}
				CPU = NULL;
			} 
		}

		// Aqui verifica si la CPU esta vacia y si es asi extrae un proceso de la cola
		// Ingresa el proceso de mayor prioridad en estado READY a la CPU
		if (CPU == NULL) {
            if (High != NULL) {
				CPU = select_highest_priority_process(High, current_time);
				if (CPU != NULL) {
					if (CPU->first_cpu_entry_time == -1) {
						CPU->first_cpu_entry_time = current_time; // flag se guarda con el current_time
						CPU->response_time = current_time - CPU->t_start; // Calcular el response time
					}
					if (current_time > CPU->t_deadline) {
						CPU->sum_deadline_time++;
					}
					strcpy(CPU->state, "RUNNING");
				}
	
            } else if (Low != NULL) {
                CPU = select_highest_priority_process(Low, current_time);
				if (CPU != NULL) {
					printf("CPU PID low %d\n",CPU->pid);
					if (CPU->first_cpu_entry_time == -1) {
						CPU->first_cpu_entry_time = current_time; // flag se guarda con el current_time
						CPU->response_time = current_time - CPU->t_start; // Calcular el response time
					}
					if (current_time > CPU->t_deadline) {
						CPU->sum_deadline_time++;
					}
					strcpy(CPU->state, "RUNNING");
				}
            }
        }

		current_time++;

	}

	print_results(output_file, processes, num_processes, current_time);
	fclose(output_file);

	// liberar memoria
	for (int i = 0; i < num_processes; i++) {
		free(processes[i]);
	}
	free(processes);

	return 0;
}

Process* select_highest_priority_process(Queue* queue, int Tactual) {
    if (queue == NULL) return NULL;

    Process* selected_process = NULL;
    int highest_priority_value = -999999;
    
    Queue* current = queue;

    while (current != NULL) {
        Process* current_process = current->process;

        if (strcmp(current_process->state, "WAITING") != 0) {
            int priority_value = (Tactual - current_process->T_LCPU) - current_process->t_deadline;

            if (priority_value > highest_priority_value ||
                (priority_value == highest_priority_value && current_process->pid < selected_process->pid)) {
                selected_process = current_process;
                highest_priority_value = priority_value;
            }
        }

        current = current->next;
    }

    return selected_process;
}

// Función para imprimir el output en el csv
void print_results(FILE *output_file, Process **processes, int num_processes, int current_time) {
    for (int i = 0; i < num_processes; i++) {
        Process *p = processes[i];
        fprintf(output_file, "%s, %d, %d, %d, %d, %d, %d\n", 
                p->name, p->pid, p->num_interruptions, p->turnaround_time, p->response_time, p->waiting_time, p->sum_deadline_time);
    }
}
