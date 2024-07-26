#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <stdint.h> // Include stdint.h for uint32_t

// Declare global_memory_pointer as an external variable
extern void *global_memory_pointer;

// Function prototypes for memory management
void *allocate_memory(uint32_t size);
void deallocate_memory(void *ptr);
void initialize_memory(); // Add the missing function prototype

#endif // MEMORY_MANAGEMENT_H
