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

	// Variables para almacenar lo datos extraidos del input_file
	char name[10];
	int pid, t_start, t_cpu_burst, n_burst, io_wait, t_deadline;

	/*Mostramos el archivo de input en consola*/
	printf("Nombre archivo: %s\n", file_name);
	printf("Cantidad de procesos: %d\n", input_file->len);
	printf("Procesos:\n");
	for (int i = 0; i < input_file->len; ++i)
	{
		strcpy(name, input_file->lines[i][0]);
		pid = atoi(input_file->lines[i][1]);
        t_start = atoi(input_file->lines[i][2]);
        t_cpu_burst = atoi(input_file->lines[i][3]);
        n_burst = atoi(input_file->lines[i][4]);
    	io_wait = atoi(input_file->lines[i][5]);
        t_deadline = atoi(input_file->lines[i][6]);
		Process* process = init_process( name, pid, t_start, t_cpu_burst, n_burst, io_wait, t_deadline);

		for (int j = 0; j < 7; ++j)
		{
			printf("%s ", input_file->lines[i][j]);
		}
		printf("\n");
	}

	input_file_destroy(input_file);
}