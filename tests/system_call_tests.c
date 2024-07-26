#include <stdio.h>
#include <assert.h>
#include <stdint.h> // Include stdint.h for uintptr_t
#include <unistd.h> // Include unistd.h for usleep
#include "../system_calls.h"
#include "../process_management.h" // Include the header file instead of the .c file
#include "../memory_management.h" // Include the memory_management.h header file

// Ensure that process_table is properly declared and accessible
extern pcb_t process_table[];

// Test system call for process creation
void test_syscall_create_process() {
    uint32_t pid = 0;
    uint32_t priority = 0;
    uint32_t entry_point = 0x1000;

    printf("Debug: Before system call - process_table[%u] details:\n", pid);
    printf("  PID: %u\n  State: %u\n  Program Counter: %p\n  Priority: %u\n  Stack Pointer: %p\n",
           process_table[pid].pid,
           process_table[pid].state,
           (void*)process_table[pid].program_counter,
           process_table[pid].priority,
           (void*)process_table[pid].stack_pointer);

    int result = handle_syscall(SYS_CREATE_PROCESS, pid, entry_point, priority);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    printf("Debug: System call result: %d\n", result);
    printf("Debug: Immediately after system call - process_table[%u] details:\n", pid);
    printf("  PID: %u\n  State: %u\n  Program Counter: %p\n  Priority: %u\n  Stack Pointer: %p\n",
           process_table[pid].pid,
           process_table[pid].state,
           (void*)process_table[pid].program_counter,
           process_table[pid].priority,
           (void*)process_table[pid].stack_pointer);

    printf("Debug: Before retry loop - Initial state: %u\n", process_table[pid].state);
    int retry_count = 0;
    const int max_retries = 10;
    const useconds_t retry_delay = 10000; // 10ms

    while (process_table[pid].state != PROCESS_STATE_READY && retry_count < max_retries) {
        usleep(retry_delay);
        retry_count++;
        printf("Debug: Retry %d - State: %u\n", retry_count, process_table[pid].state);
    }
    printf("Debug: After retry loop - Final state: %u, Retry count: %d\n", process_table[pid].state, retry_count);

    printf("Debug: After retries - process_table[%u] details:\n", pid);
    printf("  PID: %u\n  State: %u\n  Program Counter: %p\n  Priority: %u\n  Stack Pointer: %p\n",
           process_table[pid].pid,
           process_table[pid].state,
           (void*)process_table[pid].program_counter,
           process_table[pid].priority,
           (void*)process_table[pid].stack_pointer);

    // Check PROCESS_STATE_READY definition
    #ifndef PROCESS_STATE_READY
    #error "PROCESS_STATE_READY is not defined"
    #endif
    assert(PROCESS_STATE_READY == 1);

    // Ensure process table is initialized
    assert(process_table != NULL);

    // More detailed assertions
    assert(result == 0);
    assert(process_table[pid].pid == pid);
    volatile uint32_t state = process_table[pid].state;
    if (state != PROCESS_STATE_READY) {
        printf("Error: Expected process state %d, but got %d after %d retries\n", PROCESS_STATE_READY, state, retry_count);
        assert(state == PROCESS_STATE_READY);
    }
    assert(process_table[pid].program_counter == (uint32_t *)entry_point);
    assert(process_table[pid].priority == priority);
    assert(process_table[pid].stack_pointer != NULL);

    printf("test_syscall_create_process passed\n");
}

// Test system call for process termination
void test_syscall_terminate_process() {
    handle_syscall(SYS_CREATE_PROCESS, 0, (uint32_t)0x1000, 0);
    handle_syscall(SYS_TERMINATE_PROCESS, 0, 0, 0);
    assert(process_table[0].state == PROCESS_STATE_TERMINATED);
    printf("test_syscall_terminate_process passed\n");
}

// Test system call for memory allocation
void test_syscall_allocate_memory() {
    handle_syscall(SYS_ALLOCATE_MEMORY, 1024, 0, 0);
    assert(global_memory_pointer != NULL);
    printf("test_syscall_allocate_memory passed\n");
}

// Test system call for memory deallocation
void test_syscall_deallocate_memory() {
    handle_syscall(SYS_ALLOCATE_MEMORY, 1024, 0, 0);
    handle_syscall(SYS_DEALLOCATE_MEMORY, (uintptr_t)global_memory_pointer, 0, 0); // Use uintptr_t for casting
    assert(global_memory_pointer == NULL);
    printf("test_syscall_deallocate_memory passed\n");
}

// Main function to run all tests
int main(void) {
    test_syscall_create_process();
    test_syscall_terminate_process();
    test_syscall_allocate_memory();
    test_syscall_deallocate_memory();
    printf("All tests passed\n");
    return 0;
}
