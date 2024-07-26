#include <stdint.h>
#include <stddef.h> // Include stddef.h to define size_t
#include <stdio.h>  // Include stdio.h for printf
#include "../process_management.h" // Include process management header
#include "../memory_management.h" // Include memory management header

// Define system call numbers
#define SYS_WRITE 1
#define SYS_READ 2
#define SYS_CREATE_PROCESS 3
#define SYS_TERMINATE_PROCESS 4
#define SYS_ALLOCATE_MEMORY 5
#define SYS_DEALLOCATE_MEMORY 6

// Check if PROCESS_STATE_READY is defined
#ifndef PROCESS_STATE_READY
#error "PROCESS_STATE_READY is not defined"
#endif

// Function prototypes for system calls
int sys_write(int fd, const void *buf, size_t count);
int sys_read(int fd, void *buf, size_t count);
int sys_create_process(uint32_t pid, uint32_t entry_point, uint32_t priority);
int sys_terminate_process(uint32_t pid);
void *sys_allocate_memory(size_t size);
int sys_deallocate_memory(uintptr_t address);
extern void create_process(uint32_t pid, void (*entry_point)(), uint32_t priority);

// System call handler
int handle_syscall(uint32_t syscall_number, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    int result;
    switch (syscall_number) {
        case SYS_WRITE:
            result = sys_write((int)arg1, (const void *)(uintptr_t)arg2, (size_t)arg3);
            break;
        case SYS_READ:
            result = sys_read((int)arg1, (void *)(uintptr_t)arg2, (size_t)arg3);
            break;
        case SYS_CREATE_PROCESS:
            result = sys_create_process((uint32_t)arg1, (uint32_t)arg2, (uint32_t)arg3);
            break;
        case SYS_TERMINATE_PROCESS:
            result = sys_terminate_process((uint32_t)arg1);
            break;
        case SYS_ALLOCATE_MEMORY:
            result = (int)(uintptr_t)sys_allocate_memory((size_t)arg1);
            break;
        case SYS_DEALLOCATE_MEMORY:
            result = sys_deallocate_memory((uintptr_t)arg1);
            break;
        default:
            result = -1; // Invalid system call number
    }

    return result;
}

// Implementation of sys_write
int sys_write(int fd, const void *buf, size_t count) {
    // Implementation of write system call
    // For now, just return the count as a placeholder
    (void)fd; // Indicate that the parameter is intentionally unused
    (void)buf; // Indicate that the parameter is intentionally unused
    return (int)count;
}

// Implementation of sys_read
int sys_read(int fd, void *buf, size_t count) {
    // Implementation of read system call
    // For now, just return 0 as a placeholder
    (void)fd; // Indicate that the parameter is intentionally unused
    (void)buf; // Indicate that the parameter is intentionally unused
    (void)count; // Indicate that the parameter is intentionally unused
    return 0;
}

// Implementation of sys_create_process
int sys_create_process(uint32_t pid, uint32_t entry_point, uint32_t priority) {
    int result;
    printf("Debug: Entering sys_create_process - PID: %u, Entry Point: %u, Priority: %u\n", pid, entry_point, priority);
    printf("Debug: Process table state before creation - PID: %u, State: %d\n", pid, process_table[pid].state);

    if (pid >= MAX_PROCESSES) {
        printf("Error: PID %u is out of bounds (MAX_PROCESSES=%u)\n", pid, MAX_PROCESSES);
        result = -1; // Error: PID is out of bounds
    } else if (process_table[pid].state != PROCESS_STATE_NEW) {
        printf("Error: Process with PID %u already exists (state=%d)\n", pid, process_table[pid].state);
        result = -1; // Error: Process already exists
    } else {
        printf("Debug: Before create_process - Process state: %d\n", process_table[pid].state);
        printf("Debug: Calling create_process for PID: %u\n", pid);

        create_process(pid, (void (*)(void))(uintptr_t)entry_point, priority);

        printf("Debug: create_process returned. New state for PID %u: %d\n", pid, process_table[pid].state);

        if (process_table[pid].state != PROCESS_STATE_READY) {
            printf("Error: Process state not set to READY after creation. PID: %u, Current state: %d, Expected state: %d\n", pid, process_table[pid].state, PROCESS_STATE_READY);
            result = -1; // Error: Process state not set to READY after creation
        } else {
            printf("Debug: Process with PID %u successfully created and set to READY state\n", pid);
            result = 0;
        }
    }

    printf("Debug: Exiting sys_create_process - Return value: %d\n", result);
    return result;
}

// Implementation of sys_terminate_process
int sys_terminate_process(uint32_t pid) {
    terminate_process(pid);
    return 0;
}

// Implementation of sys_allocate_memory
void *sys_allocate_memory(size_t size) {
    return allocate_memory(size);
}

// Implementation of sys_deallocate_memory
int sys_deallocate_memory(uintptr_t address) {
    deallocate_memory((void *)address);
    return 0;
}
