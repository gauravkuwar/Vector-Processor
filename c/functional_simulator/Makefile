# Makefile
CC = gcc

# Compiler flags
CFLAGS = -Iinclude -Wall -Wextra

SRC = src/main.c src/hashmap.c src/instrs_maps.c # Source files
OBJ = $(SRC:.c=.o) # Object files
EXEC = funcsim

all: funcsim

# Default target
all: $(EXEC)

# Rule to link object files to create the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJ)

# Phony targets
.PHONY: all clean