#include "scheduler.h"
#include "memory.h"
#include "kernel.h"
#include "idt.h"

static process_t processes[MAX_PROCESSES];
static process_t* ready_queues[MAX_PRIORITY + 1];
static process_t* current_process = NULL;
static u32 next_pid = 1;
static bool initialized = false;

static void add_to_ready_queue(process_t* proc) {
    if (!proc) return;
    
    proc->state = PROCESS_READY;
    proc->next = ready_queues[proc->priority];
    ready_queues[proc->priority] = proc;
}

static process_t* remove_from_ready_queue(u32 priority) {
    if (priority > MAX_PRIORITY) return NULL;
    
    process_t* proc = ready_queues[priority];
    if (proc) {
        ready_queues[priority] = proc->next;
        proc->next = NULL;
    }
    return proc;
}

static process_t* find_highest_priority_process(void) {
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        if (ready_queues[i]) {
            return remove_from_ready_queue(i);
        }
    }
    return NULL;
}

static void setup_process_stack(process_t* proc, void (*entry)(void)) {
    u32* stack = (u32*)(proc->stack_base + proc->stack_size);
    
    // Initialize stack frame for iret
    stack--;
    *stack = 0x202;  // EFLAGS (IF set)
    stack--;
    *stack = 0x08;  // CS
    stack--;
    *stack = (u32)entry;  // EIP
    
    // Registers
    stack--;
    *stack = 0;  // EDI
    stack--;
    *stack = 0;  // ESI
    stack--;
    *stack = 0;  // EBP
    stack--;
    *stack = (u32)stack;  // ESP
    stack--;
    *stack = 0;  // EBX
    stack--;
    *stack = 0;  // EDX
    stack--;
    *stack = 0;  // ECX
    stack--;
    *stack = 0;  // EAX
    
    proc->esp = (u32)stack;
    proc->ebp = proc->esp;
}

void scheduler_init(void) {
    if (initialized) return;
    
    memset(processes, 0, sizeof(processes));
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        ready_queues[i] = NULL;
    }
    
    initialized = true;
}

u32 process_create(void (*entry)(void), u32 priority) {
    if (!initialized) {
        scheduler_init();
    }
    
    if (priority > MAX_PRIORITY) {
        priority = MAX_PRIORITY;
    }
    
    // Find free process slot
    process_t* proc = NULL;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_TERMINATED || processes[i].pid == 0) {
            proc = &processes[i];
            break;
        }
    }
    
    if (!proc) {
        return 0;  // No free slots
    }
    
    // Allocate stack
    proc->stack_size = 4096;  // 4KB stack
    proc->stack_base = (u32)kmalloc(proc->stack_size);
    if (!proc->stack_base) {
        return 0;
    }
    
    proc->pid = next_pid++;
    proc->priority = priority;
    proc->state = PROCESS_READY;
    
    setup_process_stack(proc, entry);
    
    add_to_ready_queue(proc);
    
    return proc->pid;
}

void process_exit(u32 pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROCESS_TERMINATED;
            if (processes[i].stack_base) {
                kfree((void*)processes[i].stack_base);
            }
            memset(&processes[i], 0, sizeof(process_t));
            break;
        }
    }
    
    if (current_process && current_process->pid == pid) {
        current_process = NULL;
        schedule();
    }
}

void schedule(void) {
    if (current_process && current_process->state == PROCESS_RUNNING) {
        add_to_ready_queue(current_process);
    }
    
    process_t* next = find_highest_priority_process();
    if (next) {
        current_process = next;
        current_process->state = PROCESS_RUNNING;
        
        // Context switch would happen here
        // For now, we'll use a simple round-robin approach
    } else if (current_process) {
        // Keep current process if no other ready
        current_process->state = PROCESS_RUNNING;
    }
}

process_t* get_current_process(void) {
    return current_process;
}

void yield(void) {
    schedule();
}
