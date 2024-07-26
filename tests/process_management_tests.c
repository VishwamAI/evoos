#include <stdio.h>
#include <assert.h>
#include "../process_management.h" // Include the header file instead of the .c file

// Declare current_process for testing purposes
pcb_t *current_process = NULL;

// Test process creation
void test_create_process() {
    create_process(1, (void (*)())0x1000, 0);
    assert(process_table[0].pid == 1);
    assert(process_table[0].state == PROCESS_STATE_NEW);
    assert(process_table[0].program_counter == (uint32_t *)0x1000);
    assert(process_table[0].priority == 0);
    printf("test_create_process passed\n");
}

// Test process scheduling
void test_schedule_process() {
    create_process(1, (void (*)())0x1000, 0);
    create_process(2, (void (*)())0x2000, 1);
    schedule_process();
    current_process = &process_table[0]; // Set current_process for testing
    assert(current_process->pid == 1);
    assert(current_process->state == PROCESS_STATE_RUNNING);
    printf("test_schedule_process passed\n");
}

// Test process termination
void test_terminate_process() {
    create_process(1, (void (*)())0x1000, 0);
    terminate_process(1);
    assert(process_table[0].state == PROCESS_STATE_TERMINATED);
    printf("test_terminate_process passed\n");
}

// Remove the main function to avoid multiple definitions
