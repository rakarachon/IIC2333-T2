#pragma once
#include <stdbool.h>
#include "process.h"

typedef struct cpu
{
  Process *process;
  bool available;
} CPU;

typedef struct cpu_list
{
  int len;
  CPU *list[MAX];
} CPU_List;

CPU_List *CPU_list_init(int n);
void CPU_list_destroy(CPU_List *list);