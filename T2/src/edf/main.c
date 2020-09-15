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
    printf("CANTIDAD DE CPUS: %d\n", n_cpu);
  }
  else
  {
    n_cpu = 1;
    printf("CANTIDAD DE CPUS: %d\n", n_cpu);
  }

  // TODO: Lectura y manejo de archivo de input
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
  CPU cpus[n_cpu];
  for (int i = 0; i < K; i+=1)
  {
    cpus[i] = (CPU) {.available = true};;
  }
  
  // Instanciar cola
  Queue q = (Queue) {.length = K};
 
  // TODO: Instanciar procesos
  // TODO: Agregar procesos a la cola
  int n_process = 0;
  while (fgets(line, sizeof(line), input_file) != NULL)
  {
    //printf("%s\n", line);
    //printf("PROCESO\n");
    token = strtok(line, s);

    int iteration = 0;
    q.processes[n_process] = (Process) {
      .state = NEW,
      .times_in_CPU = 0,
      .interruptions = 0,
      .waiting_time = 0,
      .response_time = 0,
      .current_bursts = 0
      };
    //Process process = (Process) {};

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
      }
      else if (iteration == 3)
      {
        q.processes[n_process].deadline = atoi(token);
        q.processes[n_process].priority = atoi(token);

      }
      else if (iteration == 4)
      {
        q.processes[n_process].total_bursts = atoi(token);
      }
      else if (iteration >= 5)
      {
        q.processes[n_process].bursts[iteration-5] = atoi(token);
      }


      token = strtok(NULL, s);
      iteration += 1;
    }

    //printf("CANTIDAD DE ITERACIONES: %d\n", iteration);
    n_process += 1;
  }

  fclose(input_file);

  // Inicializar variable de instante actual de tiempo
  int current_time = 0;
  int finished_processes = 0;
  Process N_highest_priority[n_cpu];

  /* INICIO SIMULACIÓN */
  while (finished_processes < K)
  {
    // TODO: Obtener N procesos más prioritarios
    get_N_highest_priority(&q, N_highest_priority, n_cpu);

    for (int i = 0; i < n_cpu; i++)
    {
      printf("%d\n", n_cpu);
      printf("NOMBRE PROCESO: %s\n", N_highest_priority[i].name);
    }
    // TODO: Revisar estado de cada proceso
    // RUNNING -> no hacer nada, pasar al siguiente proceso
    // READY -> ingresar proceso a alguna CPU
    // Si hay CPU vacía -> meter en esa CPU
    // Si NO hay CPU vacía -> buscar CPU cn proceso menos prioritario y reemplazar ese

    // Pasar a siguiente instante
    current_time++;
    // Recorrer cola de procesos y hacer cambios que correspondan luego de avanzar una unidad de tiempo
    update_processes(&q, K);
    break;
  }
  /* FIN SIMULACIÓN */

  FILE *output_file = fopen(argv[2], "w");
  fclose(output_file);
  
  return 0;
}
