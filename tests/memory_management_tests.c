#include <stdio.h>
#include <assert.h>
#include "../memory_management.h" // Include the header file instead of the .c file

// Test memory allocation
void test_allocate_memory() {
    void *mem = allocate_memory(1024);
    assert(mem != NULL);
    printf("test_allocate_memory passed\n");
}

// Test memory deallocation
void test_deallocate_memory() {
    void *mem = allocate_memory(1024);
    deallocate_memory(mem);
    // Assuming deallocate_memory sets the pointer to NULL
    assert(mem == NULL);
    printf("test_deallocate_memory passed\n");
}

// Remove the main function to avoid multiple definitions
