#include <stdbool.h>
#include "process.h"
#pragma once

typedef struct cpu
{
    Process *process;
    bool available;
} CPU;