#include "process.h"
#include "queue.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_process(Process *pr)
{
  printf("%d -- %s: ", pr->PID, pr->name);
  if (pr->state == NEW)
    printf("NEW\n");
  else if (pr->state == READY)
    printf("READY\n");
  else if (pr->state == RUNNING)
    printf("RUNNING\n");
  else if (pr->state == WAITING)
    printf("WAITING\n");
  else if (pr->state == FINISHED)
    printf("FINISHED\n");

  printf("    Deadline: %d\n", pr->deadline);

  if (pr->state != NEW)
    printf("    Start Time: %d\n", pr->start_time);
  if (pr->state == FINISHED)
    printf("    End Time: %d\n", pr->end_time);
  if (pr->state == READY)
  {
    printf("    Priority: %d\n", pr->priority);
    printf("    Time of arrival to READY queue: %d\n", pr->last_arrival_ready);
  }
  if (pr->state == RUNNING)
  {
    printf("    Priority: %d\n", pr->priority);
    printf("    Time of arrival to CPU: %d\n", pr->last_arrival_CPU);
  }
  if (pr->state == WAITING)
  {
    printf("    Time of arrival to WAITING queue: %d\n", pr->last_arrival_waiting);
  }

  if (pr->state != NEW && pr->state != FINISHED)
  {
    printf("    Total bursts: %d\n", pr->total_bursts);
    printf("    Currently in burst number %d\n", pr->current_bursts + 1);
    printf("    { ");
    for (size_t i = 0; i < pr->total_bursts; i++)
    {
      if (pr->current_bursts == i)
      {
        printf("[ %d ] ", pr->bursts[i]);
      }
      else
      {
        printf("%d ", pr->bursts[i]);
      }
    }
    printf("}\n");
    printf("    Current burst start time: %d\n", pr->current_burst_start_time);
    printf("    Total interruptions so far: %d\n", pr->interruptions);
    printf("    Waiting time so far: %d\n", pr->waiting_time);
    printf("    Response time: %d\n", pr->response_time);
    printf("    Total number of times the process has been in a CPU so far: %d\n", pr->times_in_CPU);
  }
}

Queue q_init(int K)
{
  Queue q = (Queue){
      .length = K,
      .processes = calloc(K, sizeof(Process)),
  };
  return q;
}

void q_destroy(Queue *q)
{
  if (!q)
    return;
  free(q->processes);
}

void print_queue(Queue *q)
{
  for (size_t i = 0; i < q->length; i++)
  {
    print_process(&q->processes[i]);
  }
}

int get_N_highest_priority(Queue *queue, int *N_highest, int *N)
{
  int i = 0;            /* Counter to get N processes */
  Process *CHP_process; /* Current Highest Priority Process */
  int current_idx;      /* Index of the process associated with CHP_process */
  // int selected_indexes[N]; /* Queue indexes of the processes selected with highest priority */
  int selected_counter = 0;
  bool first;
  Process *pr;
  while (i < *N)
  {
    first = true;
    // Find highest deadline that has not been selected yet
    for (size_t idx = 0; idx < queue->length; idx++)
    {
      pr = &(queue->processes[idx]);
      if (first)
      {
        if (!(pr->selected) && (pr->state == RUNNING || pr->state == READY))
        {
          CHP_process = &(queue->processes[idx]);
          current_idx = idx;
          first = false;
        }
        continue;
      }
      // Si el proceso no ha sido seleccionado
      if (!(pr->selected) && (pr->state == RUNNING || pr->state == READY))
      {
        // Si la prioridad es igual a la mayor prioridad actual, se elige el proceso de menor PID
        // Si la prioridad es mayor (i.e. más cercana a cero), se reemplaza
        if ((pr->priority == CHP_process->priority && pr->PID < CHP_process->PID) || pr->priority < CHP_process->priority)
        {
          // CHP_process = queue->processes[idx];
          CHP_process = pr;
          current_idx = idx;
        }
      }
    }
    if (!first)
    {
      queue->processes[current_idx].selected = true;
      // N_highest->processes[i] = queue->processes[current_idx];
      N_highest[i] = current_idx;
      // selected_indexes[i] = current_idx;
      selected_counter++;
    }
    i++;
  }

  // Settear selected de vuelta a False
  // for (size_t j = 0; j < selected_counter; j++)
  // {
  //   // queue->processes[selected_indexes[j]].selected = false;
  //   queue->processes[N_highest[j]].selected = false;
  // }
  return selected_counter;
}

void update_processes(Queue *queue, int n_process, int c_time, int *finished_processes, int *available_cpus, CPU_List *cpus)
{
  int cb;
  int tb;
  int cb_duration;
  int cpu_idx;
  for (int i = 0; i < n_process; i += 1)
  {
    if (queue->processes[i].state != FINISHED)
      queue->processes[i].priority -= 1;
    if (queue->processes[i].start_time == c_time)
    {
      queue->processes[i].state = READY;
      queue->processes[i].last_arrival_ready = c_time;
    }

    cb = queue->processes[i].current_bursts;
    tb = queue->processes[i].total_bursts;
    cb_duration = queue->processes[i].bursts[cb];
    if (queue->processes[i].state == RUNNING && c_time - queue->processes[i].last_arrival_CPU == cb_duration)
    {
      if (cb == tb - 1)
      {
        // Proceso terminó
        queue->processes[i].state = FINISHED;
        queue->processes[i].end_time = c_time;
        (*finished_processes)++;
      }
      else
      {
        // Proceso pasa a I/O burst
        queue->processes[i].state = WAITING;
        queue->processes[i].current_bursts++;
        queue->processes[i].current_burst_start_time = c_time;
        queue->processes[i].last_arrival_waiting = c_time;
      }
      (*available_cpus)++;
      cpu_idx = queue->processes[i].CPU_index;
      cpus->list[cpu_idx]->available = true;
      queue->processes[i].CPU_index = -1;
    }
    else if (queue->processes[i].state == WAITING && c_time - queue->processes[i].last_arrival_waiting == cb_duration)
    {
      queue->processes[i].state = READY;
      queue->processes[i].last_arrival_ready = c_time;
      queue->processes[i].waiting_time += cb_duration;
    }
  }
}