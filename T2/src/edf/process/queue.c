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
    printf("    Total response time so far: %d\n", pr->response_time);
    printf("    Total number of times the process has been in a CPU so far: %d\n", pr->times_in_CPU);
  }
}

static void processes_init(Queue *q)
{
  for (size_t i = 0; i < q->length; i++)
  {
    printf("Calling\n");
    q->processes[i] = calloc(1, sizeof(Process));
  }
}

static void processes_destroy(Queue *q)
{
  for (size_t i = 0; i < q->length; i++)
  {
    if (q->processes[i])
    {
      // free(q->processes[i]->state);
      free(q->processes[i]);
    }
  }
}

void add_process(Queue *q, int idx, char *token, int *iteration, const char *s)
{
  q->processes[idx] = &(Process){
      .state = NEW,
      .times_in_CPU = 0,
      .interruptions = 0,
      .waiting_time = 0,
      .response_time = 0,
      .current_bursts = -1};

  while (token != NULL)
  {
    if (*iteration == 0)
    {
      strcpy(q->processes[idx]->name, token);
    }
    else if (*iteration == 1)
    {
      q->processes[idx]->PID = atoi(token);
    }
    else if (*iteration == 2)
    {
      q->processes[idx]->start_time = atoi(token);
      if (atoi(token) == 0)
      {
        q->processes[idx]->state = READY;
        q->processes[idx]->last_arrival_ready = 0;
      }
    }
    else if (*iteration == 3)
    {
      q->processes[idx]->deadline = atoi(token);
      q->processes[idx]->priority = atoi(token);
    }
    else if (*iteration == 4)
    {
      q->processes[idx]->total_bursts = (atoi(token) * 2) - 1;
    }
    else if (*iteration >= 5)
    {
      q->processes[idx]->bursts[*iteration - 5] = atoi(token);
    }

    token = strtok(NULL, s);
    *iteration += 1;
  }
}

Queue *q_init(int K)
{
  Queue *q = malloc(sizeof(Queue));
  q->length = K;
  processes_init(q);
  return q;
}

void q_destroy(Queue *q)
{
  if (!q)
    return;
  processes_destroy(q);
  free(q);
}

void print_queue(Queue *q)
{
  for (size_t i = 0; i < q->length; i++)
  {
    print_process(q->processes[i]);
  }
  scanf("%s", NULL);
  printf("------------------------------------------------------------------------------------------------------\n");
}

// ------------------------------------------------------

// void get_N_highest_priority(Queue *queue, Process *N_highest, int N)
// {
//   int i = 0;               /* Counter to get N processes */
//   Process CHP_process;     /* Current Highest Priority Process */
//   int current_idx;         /* Index of the process associated with CHP_process */
//   int selected_indexes[N]; /* Queue indexes of the processes selected with highest priority */
//   bool first;
//   Process pr;
//   while (i < N)
//   {
//     first = true;
//     // Find highest deadline that has not been selected yet
//     for (size_t idx = 0; idx < queue->length; idx++)
//     {
//       pr = queue->processes[idx];
//       if (first && !pr.selected && (pr.state == RUNNING || pr.state == READY))
//       {
//         CHP_process = queue->processes[idx];
//         current_idx = idx;
//         first = false;
//         continue;
//       }
//       // Si el proceso no ha sido seleccionado
//       if (!pr.selected)
//       {
//         // Si la prioridad es igual a la mayor prioridad actual, se elige el proceso de menor PID
//         // Si la prioridad es mayor (i.e. más cercana a cero), se reemplaza
//         if ((pr.priority == CHP_process.priority && pr.PID < CHP_process.PID) || pr.priority < CHP_process.priority)
//         {
//           CHP_process = queue->processes[idx];
//           current_idx = idx;
//         }
//       }
//     }
//     queue->processes[current_idx].selected = true;
//     // N_highest[i] = CHP_process;
//     N_highest[i] = queue->processes[current_idx];
//     selected_indexes[i] = current_idx;
//     i++;
//   }

//   // Settear selected de vuelta a False
//   for (size_t j = 0; j < N; j++)
//   {
//     queue->processes[selected_indexes[j]].selected = false;
//   }
// }

// void update_processes(Queue *queue, int n_process, int c_time)
// {
//   for (int i = 0; i < n_process; i += 1)
//   {
//     queue->processes[i].priority -= 1;
//     // TODO: testear
//     if (queue->processes[i].start_time == c_time)
//     {
//       queue->processes[i].state = READY;
//     }
//   }
// }