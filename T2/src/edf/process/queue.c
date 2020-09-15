#include "process.h"
#include "queue.h"
#include <stdbool.h>
#include <stdio.h>

void get_N_highest_priority(Queue* queue, Process* N_highest, int N) {
    // Process N_highest[N]; /* N processes with highest priority */
    int i = 0; /* Counter to get N processes */
    Process CHP_process; /* Current Highest Priority Process */
    int current_idx; /* Index of the process associated with CHP_process */
    int selected_indexes[N]; /* Queue indexes of the processes selected with highest priority */
    bool first;
    Process pr;
    while (i < N)
    {
        first = true;
        // Find highest deadline that has not been selected yet
        for (size_t idx = 0; idx < queue->length; idx++)
        {
            pr = queue->processes[idx];
            if (first && !pr.selected)
            {
                CHP_process = queue->processes[idx];
                current_idx = idx;
                first = false;
                continue;
            }
            // Si el proceso no ha sido seleccionado
            if (!pr.selected)
            {
                // Si la prioridad es igual a la mayor prioridad actual, se elige el proceso de menor PID
                // Si la prioridad es mayor (i.e. más cercana a cero), se reemplaza
                if ((pr.priority == CHP_process.priority && pr.PID < CHP_process.PID) || pr.priority < CHP_process.priority)
                {
                    CHP_process = queue->processes[idx];
                    current_idx = idx;
                }

            }
        }
        queue->processes[current_idx].selected = true;
        N_highest[i] = CHP_process;
        selected_indexes[i] = current_idx;
        i++;
    }

    // TODO: settear selected de vuelta a False
}

void update_processes(Queue* queue, int n_process)
{
    printf("UPDATE");
    for (int i = 0; i < n_process; i+=1)
    {
        queue->processes[i].priority -= 1;  
    }
}