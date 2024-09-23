#pragma once
#include "../process/process.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct queue {
    Process* process;
    struct queue* next;
} Queue;

Queue* new_node(Process* process);
void add_process(Queue** head, Process* process);
Process* extract_process(Queue** head, int pid);
void free_queue(Queue* head);
void print_queue(Queue* head);
