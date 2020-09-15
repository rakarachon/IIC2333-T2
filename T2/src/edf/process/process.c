#include "process.h"

#include <stdio.h>

// void print_process(Process *pr)
// {
//     printf("%d -- %s: ", pr->PID, pr->name);
//     if (pr->state == NEW)
//         printf("NEW\n");
//     else if (pr->state == READY)
//         printf("READY\n");
//     else if (pr->state == RUNNING)
//         printf("RUNNING\n");
//     else if (pr->state == WAITING)
//         printf("WAITING\n");
//     else if (pr->state == FINISHED)
//         printf("FINISHED\n");

//     printf("    Deadline: %d\n", pr->deadline);

//     if (pr->state != NEW)
//         printf("    Start Time: %d\n", pr->start_time);
//     if (pr->state == FINISHED)
//         printf("    End Time: %d\n", pr->end_time);
//     if (pr->state == READY)
//     {
//         printf("    Priority: %d\n", pr->priority);
//         printf("    Time of arrival to READY queue: %d\n", pr->last_arrival_ready);
//     }
//     if (pr->state == RUNNING)
//     {
//         printf("    Priority: %d\n", pr->priority);
//         printf("    Time of arrival to CPU: %d\n", pr->last_arrival_CPU);
//     }
//     if (pr->state == WAITING)
//     {
//         printf("    Time of arrival to WAITING queue: %d\n", pr->last_arrival_waiting);
//     }

//     if (pr->state != NEW && pr->state != FINISHED)
//     {
//         printf("    Total bursts: %d\n", pr->total_bursts);
//         printf("    Currently in burst number %d\n", pr->current_bursts + 1);
//         printf("    { ");
//         for (size_t i = 0; i < pr->total_bursts; i++)
//         {
//             if (pr->current_bursts == i)
//             {
//                 printf("[ %d ] ", pr->bursts[i]);
//             }
//             else
//             {
//                 printf("%d ", pr->bursts[i]);
//             }
//         }
//         printf("}\n");
//         printf("    Current burst start time: %d\n", pr->current_burst_start_time);
//         printf("    Total interruptions so far: %d\n", pr->interruptions);
//         printf("    Waiting time so far: %d\n", pr->waiting_time);
//         printf("    Total response time so far: %d\n", pr->response_time);
//         printf("    Total number of times the process has been in a CPU so far: %d\n", pr->times_in_CPU);
//     }
// }
