#include "process.h"
#pragma once

#define MAX_LEN 255

typedef struct queue
{
    int length;
    Process processes[MAX_LEN];
} Queue;

void get_N_highest_priority(Queue* queue, Process* N_highest_priority, int N);

void update_processes(Queue* queue, int n_process);