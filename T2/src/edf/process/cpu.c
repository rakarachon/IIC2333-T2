#include <stdlib.h>

#include "cpu.h"

static void cpus_init(CPU_List *cpu_list)
{
  for (size_t i = 0; i < cpu_list->len; i++)
  {
    cpu_list->list[i] = calloc(1, sizeof(CPU));
    cpu_list->list[i]->available = true;
  }
}

static void cpus_destroy(CPU_List *cpu_list)
{
  for (size_t i = 0; i < cpu_list->len; i++)
  {
    if (cpu_list->list[i])
    {
      free(cpu_list->list[i]);
    }
  }
}

CPU_List *CPU_list_init(int n)
{
  CPU_List *cpu_list = malloc(sizeof(CPU_List));
  cpu_list->len = n;
  cpus_init(cpu_list);
  return cpu_list;
}

void CPU_list_destroy(CPU_List *list)
{
  if (!list)
    return;
  cpus_destroy(list);
  free(list);
}