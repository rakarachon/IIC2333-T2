#include <stdio.h> /* Input/Output */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "process/cpu.h"
#include "process/queue.h"

/* En este archivo irá el scheduler */

int main(int argc, char **argv)
{
  // Lectura de comandos
  if (argc > 4 || argc < 3)
  {
    printf("Cantidad incorrecta de comandos! :(\n");
    printf("Formato: ./edf <file> <output> [<n>]\n");
    return 1;
  }
  if (access(argv[1], F_OK) == -1)
  {
    printf("Archivo de input ingresado no fue encontrado :(\n");
    return 1;
  }
  int n_cpu;
  if (argc == 4)
  {
    n_cpu = atoi(argv[3]);
  }
  else
  {
    n_cpu = 1;
  }
  // printf("CANTIDAD DE CPUS: %d\n", n_cpu);

  // Lectura y manejo de archivo de input
  FILE *input_file = fopen(argv[1], "r");

  char n_lines[MAX];
  int K; /* Number of processes in the simulation */
  fgets(n_lines, sizeof(n_lines), input_file);
  K = atoi(n_lines);
  // printf("CANTIDAD DE PROCESOS: %d\n", K);

  char line[MAX];
  const char s[2] = " ";
  char *token;

  // Instanciar CPUs
  int available_cpus = n_cpu;
  CPU_List *cpus = CPU_list_init(n_cpu);

  // Instanciar cola
  Queue q = q_init(K);

  // Instanciar procesos y agregar a la cola
  int n_process = 0;
  while (fgets(line, sizeof(line), input_file) != NULL)
  {
    token = strtok(line, s);

    int iteration = 0;
    q.processes[n_process] = (Process){
        .state = NEW,
        .times_in_CPU = 0,
        .interruptions = 0,
        .waiting_time = 0,
        .response_time = 0,
        .current_bursts = -1,
        .was_interrupted_last_time = false,
    };

    while (token != NULL)
    {
      if (iteration == 0)
      {
        strcpy(q.processes[n_process].name, token);
      }
      else if (iteration == 1)
      {
        q.processes[n_process].PID = atoi(token);
      }
      else if (iteration == 2)
      {
        q.processes[n_process].start_time = atoi(token);
        if (atoi(token) == 0)
        {
          q.processes[n_process].state = READY;
          q.processes[n_process].last_arrival_ready = 0;
        }
      }
      else if (iteration == 3)
      {
        q.processes[n_process].deadline = atoi(token);
        q.processes[n_process].priority = atoi(token);
      }
      else if (iteration == 4)
      {
        q.processes[n_process].total_bursts = (atoi(token) * 2) - 1;
      }
      else if (iteration >= 5)
      {
        q.processes[n_process].bursts[iteration - 5] = atoi(token);
      }

      token = strtok(NULL, s);
      iteration += 1;
    }

    n_process += 1;
  }
  fclose(input_file);

  // Inicializar variable de instante actual de tiempo
  int current_time = 0;
  int finished_processes = 0;
  int *N_highest_priority = calloc(n_cpu, sizeof(int));

  int hp_idx;
  int n_ready;
  int time_spent_in_ready_q;
  /* INICIO SIMULACIÓN */
  while (finished_processes < K)
  {
    // printf("Available CPUS: %d \n", available_cpus);
    // Obtener N procesos más prioritarios
    n_ready = get_N_highest_priority(&q, N_highest_priority, &n_cpu); /* Cantidad de procesos que quieren entrar a CPU, n<=N */
    // printf("N procesos más prioritarios:\n");
    for (int i = 0; i < n_ready; i++)
    {
      hp_idx = N_highest_priority[i];
      // Revisar estado del proceso
      // print_process(&q.processes[hp_idx]);
      // printf("KKCK %d\n", N_highest_priority[n_ready]);
      if (q.processes[hp_idx].state == READY)
      {
        // printf("READY -> ingresar proceso a alguna CPU\n");
        // Si hay CPU vacía -> meter en esa CPU
        if (available_cpus > 0)
        {
          for (size_t j = 0; j < n_cpu; j++)
          {
            if (cpus->list[j]->available)
            {
              cpus->list[j]->process = &(q.processes[hp_idx]);
              cpus->list[j]->available = false;
              available_cpus--;

              // Actualizar proceso que entra a la CPU según corresponda
              cpus->list[j]->process->state = RUNNING;
              time_spent_in_ready_q = current_time - cpus->list[j]->process->last_arrival_ready;
              if (cpus->list[j]->process->times_in_CPU == 0)
              {
                // Primera vez en CPU
                cpus->list[j]->process->response_time += time_spent_in_ready_q;
              }
              cpus->list[j]->process->waiting_time += time_spent_in_ready_q;
              cpus->list[j]->process->times_in_CPU++;
              cpus->list[j]->process->last_arrival_CPU = current_time;
              cpus->list[j]->process->CPU_index = j;
              if (cpus->list[j]->process->was_interrupted_last_time)
              {
                cpus->list[j]->process->was_interrupted_last_time = false;
              }
              else
              {
                cpus->list[j]->process->current_bursts++;
              }
              cpus->list[j]->process->current_burst_start_time = current_time;
              break;
            }
          }
        }
        // Si NO hay CPU vacía -> buscar CPU con proceso menos prioritario y reemplazar ese
        else
        {
          int lowest_priority = cpus->list[0]->process->priority;
          int lpi = 0; /* lowest priority index */
          for (size_t k = 1; k < n_cpu; k++)
          {
            // printf("Proceso %d -> prioridad %d\n", cpus->list[k]->process->PID, cpus->list[k]->process->priority);
            if (cpus->list[k]->process->priority > lowest_priority && !cpus->list[k]->process->selected)
            {
              lowest_priority = cpus->list[k]->process->priority;
              lpi = k;
            }
          }
          // Cambiar estado del proceso que estaba en la CPU
          int cb = cpus->list[lpi]->process->current_bursts;             /* current burst index */
          int cb_duration = cpus->list[lpi]->process->bursts[cb];        /* current burst duration */
          int last_arrival = cpus->list[lpi]->process->last_arrival_CPU; /* Instant in which the current burst started */
          int cb_exe_time = current_time - last_arrival;                 /* Number of instants the current burst has been in a cpu executing */
          if (cb_exe_time < cb_duration)
          {
            // printf("Interrumpiendo proceso %d - %s\n", cpus->list[lpi]->process->PID, cpus->list[lpi]->process->name);
            // Si aun no termina la ráfaga actual del proceso, interrumpir
            cpus->list[lpi]->process->state = READY;
            cpus->list[lpi]->process->last_arrival_ready = current_time;
            cpus->list[lpi]->process->interruptions++;
            cpus->list[lpi]->process->was_interrupted_last_time = true;
            // Acortamos tiempo de duración de ráfaga actual para que la próxima vez que vuelva a entrar no parta la misma ráfaga desde cero
            cpus->list[lpi]->process->bursts[cb] = cb_duration - cb_exe_time;
            cpus->list[lpi]->process->CPU_index = -1;
          }
          else
          {
            // En teoría nunca debería entrar a este else
            // Pasa a un I/O burst
            // Si justo terminó la ráfaga que estaba ejecutándose, se actualiza su estado según corresponda
            cpus->list[lpi]->process->state = WAITING;
            cpus->list[lpi]->process->last_arrival_waiting = current_time;
            cpus->list[lpi]->process->current_bursts++;
            cpus->list[lpi]->process->current_burst_start_time = current_time;
            cpus->list[lpi]->process->CPU_index = -1;
          }
          // printf("Entra proceso %d - %s\n", q.processes[hp_idx].PID, q.processes[hp_idx].name);
          // Actualizar proceso que entra a la CPU según corresponda
          cpus->list[lpi]->process = &(q.processes[hp_idx]);
          cpus->list[lpi]->process->state = RUNNING;
          time_spent_in_ready_q = current_time - cpus->list[lpi]->process->last_arrival_ready;
          if (cpus->list[lpi]->process->times_in_CPU == 0)
          {
            // Primera vez en CPU
            cpus->list[lpi]->process->response_time += time_spent_in_ready_q;
          }
          cpus->list[lpi]->process->waiting_time += time_spent_in_ready_q;
          cpus->list[lpi]->process->times_in_CPU++;
          cpus->list[lpi]->process->last_arrival_CPU = current_time;
          cpus->list[lpi]->process->CPU_index = lpi;
          if (cpus->list[lpi]->process->was_interrupted_last_time)
          {
            cpus->list[lpi]->process->was_interrupted_last_time = false;
          }
          else
          {
            cpus->list[lpi]->process->current_bursts++;
          }
          cpus->list[lpi]->process->current_burst_start_time = current_time;
        }
      }
    }
    for (int i = 0; i < n_ready; i++)
    {
      q.processes[N_highest_priority[i]].selected = false;
    }
    // Pasar a siguiente instante
    current_time++;
    // Recorrer cola de procesos y hacer cambios que correspondan luego de avanzar una unidad de tiempo
    update_processes(&q, K, current_time, &finished_processes, &available_cpus, cpus);
  }
  /* FIN SIMULACIÓN */

  // Output file
  FILE *output_file = fopen(argv[2], "w");
  int dl; /* Indica si cumplió deadline */
  for (int i = 0; i < q.length; i++)
  {
    if (q.processes[i].end_time > q.processes[i].deadline)
    {
      dl = 0;
    }
    else
    {
      dl = 1;
    }
    fprintf(
        output_file,
        "%s,%d,%d,%d,%d,%d,%d\n",
        q.processes[i].name,
        q.processes[i].times_in_CPU,
        q.processes[i].interruptions,
        q.processes[i].end_time - q.processes[i].start_time,
        q.processes[i].response_time,
        q.processes[i].waiting_time,
        dl);
  }

  fclose(output_file);

  free(N_highest_priority);
  q_destroy(&q);
  CPU_list_destroy(cpus);
  return 0;
}
