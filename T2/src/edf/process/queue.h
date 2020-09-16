#pragma once
#include "process.h"
#include "cpu.h"

#define MAX_LEN 255

typedef struct queue
{
  int length;
  Process *processes;
} Queue;

Queue q_init(int K);
void q_destroy(Queue *q);
void print_process(Process *pr);
void print_queue(Queue *q);
void add_process(Queue *q, int idx, char *token, int *iteration, const char *s);
int get_N_highest_priority(Queue *queue, int *N_highest_priority, int *N);
void update_processes(Queue *queue, int n_process, int c_time, int *finished_processes, int *available_cpus, CPU_List *cpus);