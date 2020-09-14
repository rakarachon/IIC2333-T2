#include <process.h>

#define MAX_LEN 255

typedef struct queue
{
    int length;
    Process *processes[MAX_LEN];
} Queue;