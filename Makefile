# Makefile for AI Kernel

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -nostdlib -Wl,-e,_start
TEST_LDFLAGS = -lc

# Directories
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
KERNEL_OBJS = $(OBJS) $(OBJ_DIR)/bootstrap.o

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_SRCS)/%.c, $(OBJ_DIR)/%.o, $(TEST_SRCS))
TEST_BIN = test_kernel

# Targets
all: kernel

kernel: $(KERNEL_OBJS)
	$(CC) $(LDFLAGS) -o kernel $(KERNEL_OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(filter-out $(OBJ_DIR)/bootstrap.o, $(OBJS)) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_LDFLAGS) -o $(TEST_BIN) $(filter-out $(OBJ_DIR)/bootstrap.o, $(OBJS)) $(TEST_OBJS)
	./$(TEST_BIN)

clean:
	rm -f $(OBJ_DIR)/*.o kernel $(TEST_BIN)

.PHONY: all kernel test clean

# Ensure obj directory exists
$(shell mkdir -p $(OBJ_DIR))

$(OBJ_DIR)/bootstrap.o: bootstrap.c
	$(CC) $(CFLAGS) -c $< -o $@
