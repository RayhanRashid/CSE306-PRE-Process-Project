# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c11

# Output executable name
TARGET = csv_parser

# Source files
SRC = csv.c

# Object files
OBJ = $(SRC:.c=.o)

# Default rule: compile the program
all: $(TARGET)

# Compile the program
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean compiled files
clean:
	rm -f $(OBJ) $(TARGET)


