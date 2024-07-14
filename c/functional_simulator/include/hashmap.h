#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct HashmapNode {
    char* key;
    char* val;
    struct HashmapNode* next;
} HashmapNode;

typedef struct Hashmap {
    struct HashmapNode* nodes;
    int size;
} Hashmap;


uint32_t hash(char* s);
Hashmap* init_map(char* array[][2], int size);
void free_map(Hashmap* map);
bool map_contains(char* target_key, Hashmap* map);
char* map_get(char* target_key, Hashmap* map);

#endif // HASHMAP_H