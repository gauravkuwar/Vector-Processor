# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
HELPERS_DIR = ../helpers

all: funcsim

funcsim: main.o $(HELPERS_DIR)/container.o
	$(CC) $(CFLAGS) -o funcsim main.o $(HELPERS_DIR)/container.o

main.o: main.c $(HELPERS_DIR)/container.h
	$(CC) $(CFLAGS) -c main.c

$(HELPERS_DIR)/container.o: $(HELPERS_DIR)/container.c $(HELPERS_DIR)/container.h
	$(CC) $(CFLAGS) -c $(HELPERS_DIR)/container.c -o $(HELPERS_DIR)/container.o

clean:
	rm -f *.o $(HELPERS_DIR)/*.o funcsim