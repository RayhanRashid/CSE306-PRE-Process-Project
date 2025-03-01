# OS identification from: https://stackoverflow.com/questions/714100/os-detecting-makefile
OS := $(shell uname -s)

ifeq ($(OS), Darwin)
  INCLUDE_PATH := /opt/homebrew/Cellar/criterion/2.4.1_1/include
  LIB_PATH := /opt/homebrew/Cellar/criterion/2.4.1_1/lib
endif
ifeq ($(OS), Linux)
  INCLUDE_PATH := /util/criterion/include
  LIB_PATH := /util/criterion/lib/x86_64-linux-gnu
endif

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c11

# Output executable name
TARGET = csv

# Source files
SRC = csv.c

# Object files
OBJ = $(SRC:.c=.o)

# Test files
TEST_SRC = test_csv.c
TEST_BIN = test_runner

# Criterion Library
TEST_FLAGS = -lcriterion

# Default rule: compile the program
all: $(TARGET)

# Compile the program
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

.PHONY : test clean

# Runs shell tests based on handout
test: all
	sh example_tests.sh

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean compiled files
clean:
	rm -f $(OBJ) $(TARGET)


