#include <sys/types.h> /* Primitive System Data Types */
#include <stdbool.h>
#pragma once

#define MAX 255

typedef enum states
{
    NEW,
    READY,
    RUNNING,
    WAITING,
    FINISHED,
} State;

typedef struct process
{
    pid_t PID;                    /* Process ID */
    char name[32];                /* Process Name */
    State state;                  /* Current state */
    int priority;                 /* Instants left until deadline. The closer to 0, the higher it is. */
    int deadline;                 /* Specific time in which the process must have finished */
    int start_time;               /* Specific time in which the process arrives to the queue */
    int end_time;                 /* Specific time in which the process changes to state FINISHED */
    int total_bursts;             /* Total amount of bursts of the process (CPU + I/O) */
    int bursts[MAX];              /* Array containing the duration of each burst */
    int current_bursts;           /* Number of bursts finished so far */
    int current_burst_start_time; /* Instant in which the current burst started */
    int interruptions;            /* Number of times the process has been interrupted */
    int last_arrival_ready;       /* Specific time of the last change of the process state to READY */
    int last_arrival_CPU;         /* Specific time of the last arrival to a CPU */
    // TODO: revisar si last_arrival_waiting es necesario
    int last_arrival_waiting; /* Specific time of the last change of the process state to WAITING */
    int waiting_time;         /* Time spent in state WAITING plus state READY */
    // TODO: Averiguar si response time es suma de todos los tiempos que pasa en Ready, o solo la primera vez que llega a Ready
    int response_time; /* Time spent in state READY */
    int times_in_CPU;  /* Number of times the process has used a CPU */
    bool selected;     /* True if process has been selected as one of the N processes with highest priority */
    bool was_interrupted_last_time;
} Process;

Process *process_init();
// void print_process(Process *pr);