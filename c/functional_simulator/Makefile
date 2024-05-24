# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
HELPERS_DIR = ../helpers

all: funcsim

funcsim: main.o $(HELPERS_DIR)/set.o
	$(CC) $(CFLAGS) -o funcsim main.o $(HELPERS_DIR)/set.o

main.o: main.c $(HELPERS_DIR)/set.h
	$(CC) $(CFLAGS) -c main.c

$(HELPERS_DIR)/set.o: $(HELPERS_DIR)/set.c $(HELPERS_DIR)/set.h
	$(CC) $(CFLAGS) -c $(HELPERS_DIR)/set.c -o $(HELPERS_DIR)/set.o

clean:
	rm -f *.o funcsim