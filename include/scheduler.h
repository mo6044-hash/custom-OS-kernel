#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "kernel.h"

#define MAX_PROCESSES 64
#define MAX_PRIORITY 3

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef struct process {
    u32 pid;
    u32 priority;
    process_state_t state;
    u32 esp;
    u32 ebp;
    u32 eip;
    u32 stack_base;
    u32 stack_size;
    struct process* next;
} process_t;

void scheduler_init(void);
u32 process_create(void (*entry)(void), u32 priority);
void process_exit(u32 pid);
void schedule(void);
process_t* get_current_process(void);
void yield(void);

#endif
