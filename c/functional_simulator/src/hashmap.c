// Basically a immutable hashmap
// purpose is to check if a value exists in the hashmap
// and get mapped value
// efficiently using hashes

#include "../include/hashmap.h"

uint32_t hash(char* s) {
    const uint32_t FNV_prime = 16777619;
    const uint32_t offset_basis = 2166136261;
    uint32_t hash = offset_basis;

    while (*s) {
        hash ^= (uint8_t)*s;   // XOR the lower 8 bits with the current byte of the string
        hash *= FNV_prime;     // Multiply by FNV prime
        s++;                   // Move to the next character
    }

    return hash;
}

Hashmap* init_map(char* array[][2], int size) {
    Hashmap* map = (Hashmap*)malloc(sizeof(Hashmap));
    if (map == NULL) {
        printf("Memory allocation for hashmap failed\n");
        return NULL;
    }

    map->nodes = (HashmapNode*)malloc(size * sizeof(HashmapNode));
    if (map->nodes == NULL) {
        printf("Memory allocation for hashmap failed\n");
        return NULL;
    }

    map->size = size;

    // initialize all keys to NULL
    for (int i=0; i<size; i++) {
        map->nodes[i].key = NULL;
    }

    HashmapNode* cur;
    int idx;
    char* key;
    char* val;

    // input values in array into the hashmap
    for (int i=0; i<size; i++) {
        key = array[i][0];
        val = array[i][1];
        idx = hash(key) % map->size; // get index from hash of key (element 1)
        cur = &map->nodes[idx];
        
        while (cur->key != NULL) {
            printf("Here\n");
            if (strcmp(key, cur->key) == 0) {
                cur->key = key;
                cur->val = val;
                continue;
            }
            cur = cur->next;
        }

        // if there is no cur->key == key in hashmap linked list
        cur->key = key;
        cur->val = val;

        // allocate memory for new node and set it to last node in the linked list
        HashmapNode* node = (HashmapNode*)malloc(sizeof(HashmapNode));
        if (node == NULL) {
            printf("Memory allocation for new HashmapNode failed\n");
            return NULL;
        }

        node->key = NULL;
        cur->next = node;
    }

    return map;
}

void free_map(Hashmap* map) {
    // free nodes
    HashmapNode* cur;
    HashmapNode* nxt;

    for (int i=0; i<map->size; i++) {
        cur = map->nodes[i].next; // skip first node
        while (cur != NULL) {
            nxt = cur->next;
            free(cur);
            cur = nxt;
        }
        free(cur); // free last NULL node
    }

    free(map->nodes); // free array of nodes
    free(map); // fre hashmap struct
};

bool map_contains(char* target_key, Hashmap* map) {
    int idx = hash(target_key) % map->size; // get index from hash of val
    HashmapNode* cur = &map->nodes[idx];

    // check if key in node linked list is equal to the val
    while (cur->key != NULL) {
        if (strcmp(target_key, cur->key) == 0)
            return true;

        cur = cur->next;   
    }
    return false;
}

char* map_get(char* target_key, Hashmap* map) {
    int idx = hash(target_key) % map->size; // get index from hash of val
    HashmapNode* cur = &map->nodes[idx];

    // check if key in node linked list is equal to the val
    while (cur->key != NULL) {
        if (strcmp(target_key, cur->key) == 0)
            return cur->val;

        cur = cur->next;   
    }
    return false;
}


// for testing purposes
int main() {
    char *words_array[][2] = {
        {"ab", ""}, 
        {"ba", ""}, 
        {"cd", ""},
        {"ef", ""},
        {"ksed", ""},
        {"djwe", "1"},
        {"weu", "2"},
        {"1", "1"},
        {"1", "2"},
    };

    size_t size = sizeof(words_array) / sizeof(words_array[0]);

    printf("Initializing hashmap\n");
    // init_map(words, words_array, size);
    Hashmap *words = init_map(words_array, size);
    HashmapNode* cur;
    bool has;
    printf("Initialized hashmap\n");

    // visualize hashmap with linked list of nodes
    for (int i=0; i<size; i++) {
        printf("%i: ", i);
        cur = &words->nodes[i];
        while (cur->key != NULL) {
            has = map_contains(cur->key, words);
            // bool has = 0;
            printf("%s (%d) -> ", cur->key, has);
            cur = cur->next;
        }
        printf("NULL \n");
    }

    // don't have
    // printf("%s = (%d)\n", "ab1c", map_contains("abc1", words));
    // printf("%s = (%d)\n", "wed", map_contains("wde", words));
    // printf("%s = (%d)\n", "cab", map_contains("cab", words));

    free_map(words);
    return 0;
}