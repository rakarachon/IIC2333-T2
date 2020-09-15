#include <stdio.h> /* Input/Output */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "process/cpu.h"
#include "process/queue.h"

/* En este archivo irá el scheduler */

// TODO: Borrar prints de debuggeo

int main(int argc, char **argv)
{
  // Lectura de comandos
  if (argc > 4 || argc < 3)
  {
    printf("Cantidad incorrecta de comandos! :(\n");
    printf("Formato: ./edf <file> <output> [<n>]\n");
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
  printf("CANTIDAD DE CPUS: %d\n", n_cpu);

  // Lectura y manejo de archivo de input
  // TODO: Sera necesario agregar una verificación de que el archivo de input entregado existe? para que no tire segmentation fault en caso que no exista, o podremos asumir que siempre se entregara un nombre archivo de input valido?
  FILE *input_file = fopen(argv[1], "r");

  char n_lines[MAX];
  int K; /* Number of processes in the simulation */
  fgets(n_lines, sizeof(n_lines), input_file);
  K = atoi(n_lines);
  printf("CANTIDAD DE PROCESOS: %d\n", K);

  char line[MAX];
  const char s[2] = " ";
  char *token;

  // Instanciar CPUs
  int available_cpus = n_cpu;
  CPU_List *cpus = CPU_list_init(n_cpu);

  // Instanciar cola
  Queue q = q_init(K);

  // TODO: Delete later
  printf("Cola Vacía:\n");
  print_queue(&q);

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

  // TODO: Delete later
  printf("Cola lista para iniciar simulación\n");
  print_queue(&q);

  // Inicializar variable de instante actual de tiempo
  int current_time = 0;
  // int finished_processes = 0;
  // // Process *N_highest_priority = calloc(n_cpu, sizeof(Process));
  // Process *N_highest_priority[n_cpu];

  // // TODO: Delete this variable later
  // char input[1];
  // /* INICIO SIMULACIÓN */
  // while (finished_processes < K)
  // {
  //   // Obtener N procesos más prioritarios
  //   get_N_highest_priority(&q, *N_highest_priority, n_cpu);
  //   int time_spent_in_ready_q;
  //   for (int i = 0; i < n_cpu; i++)
  //   {
  //     // Process current_pr = N_highest_priority[i];
  //     // printf("PROCESO %d: %s\n", current_pr.PID, current_pr.name);
  //     // Revisar estado del proceso
  //     if (N_highest_priority[i]->state == READY)
  //     {
  //       // READY -> ingresar proceso a alguna CPU
  //       // printf("  READY\n");

  //       // Si hay CPU vacía -> meter en esa CPU
  //       if (available_cpus > 0)
  //       {
  //         for (size_t j = 0; j < n_cpu; j++)
  //         {
  //           if (cpus[j].available)
  //           {
  //             // cpus[j].process = &current_pr;
  //             cpus[j].process = N_highest_priority[j];
  //             cpus[j].available = false;
  //             available_cpus--;

  //             // Actualizar proceso que entra a la CPU según corresponda
  //             cpus[j].process->state = RUNNING;
  //             time_spent_in_ready_q = current_time - cpus[j].process->last_arrival_ready;
  //             cpus[j].process->response_time = cpus[j].process->response_time + time_spent_in_ready_q;
  //             cpus[j].process->times_in_CPU++;
  //             cpus[j].process->last_arrival_CPU = current_time;
  //             cpus[j].process->current_bursts++;
  //             cpus[j].process->current_burst_start_time = current_time;
  //             break;
  //           }
  //         }
  //       }
  //       // Si NO hay CPU vacía -> buscar CPU con proceso menos prioritario y reemplazar ese
  //       else
  //       {
  //         int lowest_priority = cpus[0].process->priority;
  //         int lpi = 0; /* lowest priority index */
  //         for (size_t i = 1; i < n_cpu; i++)
  //         {
  //           if (cpus[i].process->priority > lowest_priority)
  //           {
  //             lowest_priority = cpus[i].process->priority;
  //             lpi = i;
  //           }
  //         }

  //         // Cambiar estado del proceso que estaba en la CPU
  //         int cb = cpus[lpi].process->current_bursts;             /* current burst index */
  //         int cb_duration = cpus[lpi].process->bursts[cb];        /* current burst duration */
  //         int last_arrival = cpus[lpi].process->last_arrival_CPU; /* Instant in which the current burst started */
  //         int cb_exe_time = current_time - last_arrival;          /* Number of instants the current burst has been in a cpu executing */
  //         if (cb_exe_time < cb_duration)
  //         {
  //           // Si aun no termina la ráfaga actual del proceso, interrumpir
  //           cpus[lpi].process->state = READY;
  //           cpus[lpi].process->last_arrival_ready = current_time;
  //           cpus[lpi].process->interruptions++;
  //           // Acortamos tiempo de duración de ráfaga actual para que la próxima vez que vuelva a entrar no parta la misma ráfaga desde cero
  //           cpus[lpi].process->bursts[cb] = cb_duration - cb_exe_time;
  //         }
  //         else
  //         {
  //           // Caso 1: pasa a un i/o burst
  //           //TODO: Caso 2: termino todos los burst
  //           // TODO: Caso 3: proceso pasa a i/o burst pero no es revisado en esta parte
  //           // TODO: Caso 4: proceso pasa a finished pero no es revisado en esta parte
  //           // Si justo terminó la ráfaga que estaba ejecutándose, se actualiza su estado según corresponda
  //           cpus[lpi].process->state = WAITING;
  //           cpus[lpi].process->last_arrival_waiting = current_time;
  //           cpus[lpi].process->current_bursts++;
  //           cpus[lpi].process->current_burst_start_time = current_time;
  //         }

  //         // Cambiar estado del proceso que entra a la CPU
  //         cpus[lpi].process = N_highest_priority[i];
  //         // TODO: Creo que estas 2 líneas no son necesarias
  //         // cpus[lpi].available = false;
  //         // available_cpus--;

  //         // Actualizar proceso que entra a la CPU según corresponda
  //         cpus[lpi].process->state = RUNNING;
  //         time_spent_in_ready_q = current_time - cpus[i].process->last_arrival_ready;
  //         cpus[i].process->response_time = cpus[i].process->response_time + time_spent_in_ready_q;
  //         cpus[i].process->times_in_CPU++;
  //         cpus[i].process->last_arrival_CPU = current_time;
  //         cpus[i].process->current_bursts++;
  //         cpus[i].process->current_burst_start_time = current_time;
  //       }
  //     }
  //     // else
  //     // {
  //     //   // RUNNING -> no hacer nada, pasar al siguiente proceso
  //     //   printf("  RUNNING\n");
  //     // }
  //   }
  //   printf("CURRENT TIME: %d\n", current_time);
  //   print_queue(q);
  //   scanf("%s", input);
  //   // Pasar a siguiente instante
  //   current_time++;
  //   // Recorrer cola de procesos y hacer cambios que correspondan luego de avanzar una unidad de tiempo
  //   update_processes(&q, K, current_time);
  //   // break;
  // }
  // /* FIN SIMULACIÓN */

  // // TODO: Output file
  // // FILE *output_file = fopen(argv[2], "w");
  // // fclose(output_file);

  // // free(N_highest_priority);

  // TODO: A lo mejor será necesario vaciar CPUs antes de destruirlas para que dsps no haya topes al hacer free a los procesos de la queue
  q_destroy(&q);
  CPU_list_destroy(cpus);
  return 0;
}
