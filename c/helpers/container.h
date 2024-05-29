#ifndef CONTAINER_H
#define CONTAINER_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ContainerNode {
    char* key;
    struct ContainerNode* next;
} ContainerNode;

typedef struct Container {
    struct ContainerNode* nodes;
    int size;
} Container;


uint32_t hash(char* s);
bool contains(char* val, Container* container);
void free_container(Container* container);
Container* init_container(char **array, int size);


#endif // CONTAINER_H