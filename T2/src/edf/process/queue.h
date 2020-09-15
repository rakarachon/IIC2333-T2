#include "process.h"
#pragma once

#define MAX_LEN 255

typedef struct queue
{
  int length;
  Process *processes[MAX_LEN];
} Queue;

Queue *q_init(int K);
void q_destroy(Queue *q);
void print_process(Process *pr);
void print_queue(Queue *q);
void add_process(Queue *q, int idx, char *token, int *iteration, const char *s);

void get_N_highest_priority(Queue *queue, Process *N_highest_priority, int N);
void update_processes(Queue *queue, int n_process, int c_time);