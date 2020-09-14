#include <stdbool.h>
#include <process.h>

typedef struct cpu
{
    Process *process;
    bool available;
} CPU;