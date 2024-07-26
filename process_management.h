#ifndef PROCESS_MANAGEMENT_H
#define PROCESS_MANAGEMENT_H

#include <stdint.h> // Include stdint.h for uint32_t

// Process states
#define PROCESS_STATE_NEW 0
#define PROCESS_STATE_READY 1
#define PROCESS_STATE_RUNNING 2
#define PROCESS_STATE_WAITING 3
#define PROCESS_STATE_TERMINATED 4

// Maximum number of processes
#define MAX_PROCESSES 256

// Process Control Block (PCB) structure
typedef struct pcb {
    uint32_t pid; // Process ID
    uint32_t state; // Process state
    uint32_t *stack_pointer; // Stack pointer
    uint32_t *program_counter; // Program counter
    uint32_t priority; // Process priority
    uint32_t stack[1024]; // Stack space for the process
} pcb_t;

// Function prototypes for process management
int create_process(uint32_t pid, void (*entry_point)(), uint32_t priority);
void schedule_process();
void terminate_process(uint32_t pid);
pcb_t* schedule_next_process();
void context_switch(pcb_t *current_process);

// External variables
extern pcb_t process_table[];
extern pcb_t *current_process;

#endif // PROCESS_MANAGEMENT_H
