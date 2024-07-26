#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

#include <stdint.h> // Include stdint.h to define uint32_t

// System call numbers
#define SYS_CREATE_PROCESS 1
#define SYS_TERMINATE_PROCESS 2
#define SYS_ALLOCATE_MEMORY 3
#define SYS_DEALLOCATE_MEMORY 4

// Function prototypes for system calls
int handle_syscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3);

#endif // SYSTEM_CALLS_H
