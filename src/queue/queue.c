#include "queue.h"

Queue* create_node(Process* process) {
    Queue* new_node = (Queue*)malloc(sizeof(Queue));
    new_node->process = process;
    new_node->next = NULL;
    return new_node;
}

void add_process(Queue** head, Process* process) {
    Queue* new_node = create_node(process);
    
    if (*head == NULL) {
        *head = new_node;
    } else {
        Queue* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

Process* extract_process(Queue** head, int pid) {
    Queue* current = *head;
    Queue* previous = NULL;

    while (current != NULL) {
        if (current->process->pid == pid) {
            if (previous == NULL) {
                *head = current->next;
            } else {
                previous->next = current->next;
            }

            Process* extracted_process = current->process;
            free(current);
            return extracted_process;
        }
        previous = current;
        current = current->next;
    }

    return NULL;
}

void free_queue(Queue* head) {
    Queue* current = head;
    while (current != NULL) {
        Queue* next = current->next;
        free(current);                    
        current = next;
    }
}

void print_queue(Queue* head) {
    Queue* current = head;
    while (current != NULL) {
        printf("Nombre: %s, PID: %d\n", current->process->name, current->process->pid);
        current = current->next;
    }
}
