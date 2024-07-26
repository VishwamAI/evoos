/*
 * Kernel Architecture Decision: Microkernel
 *
 * The kernel architecture for this project is based on a microkernel design.
 * This decision was made to ensure modularity, security, and maintainability,
 * which are critical for AI Laptops and Android OS development.
 *
 * Benefits of Microkernel Architecture:
 * - Modularity: Core functionalities are separated into distinct modules,
 *   allowing for easier updates and maintenance.
 * - Security: By running most services in user space, the microkernel reduces
 *   the risk of system-wide crashes and security breaches.
 * - Performance: The microkernel design allows for fine-tuned optimizations
 *   specific to AI applications, such as efficient handling of large datasets
 *   and low-latency operations.
 *
 * This architecture aligns with the unique requirements of AI Laptops and
 * Android OS, providing a robust foundation for future development.
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "../process_management.h"

// Function prototype for the updated create_process function
int create_process(uint32_t pid, void (*entry_point)(), uint32_t priority);

#define KERNEL_FALSE 0

// Define process states
#define PROCESS_STATE_NEW 0
#define PROCESS_STATE_READY 1
#define PROCESS_STATE_RUNNING 2
#define PROCESS_STATE_WAITING 3
#define PROCESS_STATE_TERMINATED 4

// Define process table
#define MAX_PROCESSES 256
pcb_t process_table[MAX_PROCESSES];

// Implement process creation
int create_process(uint32_t pid, void (*entry_point)(), uint32_t priority) {
    __sync_synchronize();
    printf("Debug: create_process - Entry. PID: %d, Entry Point: %p, Priority: %d\n", pid, (void*)entry_point, priority);

    if (pid >= MAX_PROCESSES) {
        printf("Error: PID %d is out of bounds\n", pid);
        return -1;
    }

    pcb_t *process = &process_table[pid];

    // Initialize process state to NEW
    __atomic_store_n(&process->state, PROCESS_STATE_NEW, __ATOMIC_SEQ_CST);
    printf("Debug: create_process - State initialized to NEW. PID: %d, State: %d\n", pid, __atomic_load_n(&process->state, __ATOMIC_SEQ_CST));

    // Check initial state atomically
    uint32_t initial_state = __atomic_load_n(&process->state, __ATOMIC_SEQ_CST);
    printf("Debug: create_process - Initial state: %d\n", initial_state);

    if (initial_state != PROCESS_STATE_NEW) {
        printf("Error: Process with PID %d already exists (state: %d)\n", pid, initial_state);
        return -2;
    }

    // Initialize process fields
    process->pid = pid;
    process->program_counter = (uint32_t *)entry_point;
    process->priority = priority;
    process->stack_pointer = &process->stack[1023];

    // Initialize stack
    memset(process->stack, 0, sizeof(process->stack));

    printf("Debug: create_process - Process fields set. PID: %d, PC: %p, Priority: %d\n",
           process->pid, (void*)process->program_counter, process->priority);

    // Set the state to READY after initializing other fields using atomic operation
    uint32_t expected = PROCESS_STATE_NEW;
    uint32_t desired = PROCESS_STATE_READY;
    printf("Debug: create_process - Before atomic state change. PID: %d, Current State: %d\n", pid, __atomic_load_n(&process->state, __ATOMIC_SEQ_CST));
    printf("Debug: create_process - Attempting atomic state change. Expected: %d, Desired: %d\n", expected, desired);

    bool success = __atomic_compare_exchange_n(&process->state, &expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

    uint32_t final_state = __atomic_load_n(&process->state, __ATOMIC_SEQ_CST);
    printf("Debug: create_process - After atomic state change. PID: %d, Success: %s, New State: %d\n", pid, success ? "Yes" : "No", final_state);
    printf("Debug: create_process - Atomic state change result: %s. Final state: %d\n", success ? "Success" : "Failure", final_state);

    if (!success) {
        printf("Error: Failed to set process state to READY for PID %d\n", pid);
        return -3;
    }

    // Verify all fields after initialization
    printf("Debug: create_process - Before final verification. PID: %d, State: %d, PC: %p, Priority: %d\n", pid, final_state, (void*)process->program_counter, process->priority);
    if (process->pid != pid || process->program_counter != (uint32_t *)entry_point ||
        process->priority != priority || process->stack_pointer != &process->stack[1023] ||
        final_state != PROCESS_STATE_READY) {
        printf("Critical Error: Process fields do not match expected values after initialization for PID %d\n", pid);
        return -4;
    }

    printf("Debug: create_process - Process creation completed for PID %d. Final state: %d\n",
           process->pid, final_state);
    return 0;
}

// Implement process scheduling
void schedule_process() {
    pcb_t *next_process = schedule_next_process();
    if (next_process != NULL) {
        next_process->state = PROCESS_STATE_RUNNING;
        // Switch to the selected process (context switch)
        context_switch(next_process);
    }
}

// Implement process termination
void terminate_process(uint32_t pid) {
    if (pid >= MAX_PROCESSES) {
        return;
    }

    pcb_t *process = &process_table[pid];

    if (process->state == PROCESS_STATE_TERMINATED) {
        return;
    }

    process->state = PROCESS_STATE_TERMINATED;

    // Clean up process-specific information
    // For example, you might want to free any dynamically allocated memory here
    // TODO: Implement custom memory clearing function if needed
}

// Implement context switch using the custom scheduler
void context_switch(pcb_t *current_process) {
    printf("Debug: context_switch - Entry\n");
    if (current_process != NULL) {
        printf("Debug: context_switch - Current process PID: %d, State: %d\n", current_process->pid, current_process->state);
    } else {
        printf("Debug: context_switch - No current process\n");
    }

    pcb_t *next_process = schedule_next_process();

    if (next_process != NULL) {
        printf("Debug: context_switch - Next process PID: %d, State: %d\n", next_process->pid, next_process->state);
        printf("Debug: context_switch - Changing process states\n");
        if (current_process != NULL) {
            current_process->state = PROCESS_STATE_READY;
            printf("Debug: context_switch - Updated current process state: %d\n", current_process->state);
        }
        next_process->state = PROCESS_STATE_RUNNING;
        printf("Debug: context_switch - Updated next process state: %d\n", next_process->state);

        // Perform actual context switch here (save/restore registers, etc.)
        // This is typically done in assembly or with architecture-specific instructions
        // TODO: Implement actual context switch
    } else {
        printf("Debug: context_switch - No next process found\n");
    }
}

// Define a custom scheduler function
pcb_t* schedule_next_process() {
    static uint32_t current_process_index = 0;
    printf("Debug: schedule_next_process - Entry\n");
    printf("Debug: schedule_next_process - Current process index: %d\n", current_process_index);

    // Example scheduling algorithm: Round-robin scheduling
    for (uint32_t i = 0; i < MAX_PROCESSES; i++) {
        current_process_index = (current_process_index + 1) % MAX_PROCESSES;
        printf("Debug: schedule_next_process - Checking process %d, state: %d\n", current_process_index, process_table[current_process_index].state);
        if (process_table[current_process_index].state == PROCESS_STATE_READY) {
            printf("Debug: schedule_next_process - Returning process %d\n", current_process_index);
            return &process_table[current_process_index];
        }
    }
    printf("Debug: schedule_next_process - No ready process found\n");
    return NULL; // No ready process found
}
