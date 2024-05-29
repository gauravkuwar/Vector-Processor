// Basically a immutable set
// purpose is to check if a value exists in the set
// efficiently using hashes

#include "container.h"

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

bool contains(char* val, Container* container) {
    int idx = hash(val) % container->size; // get index from hash of val
    ContainerNode* cur = &container->nodes[idx];

    // check if key in node linked list is equal to the val
    while (cur->key != NULL) {
        if (strcmp(val, cur->key) == 0)
            return true;

        cur = cur->next;   
    }
    return false;
}

void free_container(Container* container) {
    // free nodes
    ContainerNode* cur;
    ContainerNode* nxt;

    for (int i=0; i<container->size; i++) {
        cur = container->nodes[i].next; // skip first node
        while (cur != NULL) {
            nxt = cur->next;
            free(cur);
            cur = nxt;
        }
        free(cur); // free last NULL node
    }

    free(container->nodes); // free array of nodes
    free(container); // fre container struct
};

Container* init_container(char **array, int size) {
    Container* container = (Container*)malloc(sizeof(Container));
    if (container == NULL) {
        printf("Memory allocation for container failed\n");
        return NULL;
    }

    container->nodes = (ContainerNode*)malloc(size * sizeof(ContainerNode));
    if (container->nodes == NULL) {
        printf("Memory allocation for ContainerNode failed\n");
        return NULL;
    }

    container->size = size;

    // initialize all keys to NULL
    for (int i=0; i<size; i++) {
        container->nodes[i].key = NULL;
    }

    ContainerNode* cur;
    int idx;

    // input values in array into the container
    for (int i=0; i<size; i++) {
        idx = hash(array[i]) % container->size; // get index from hash of val
        cur = &container->nodes[idx];
        
        while (cur->key != NULL) {
            if (strcmp(array[i], cur->key) == 0) {
                cur->key = array[i];
                continue;
            }
            cur = cur->next;
        }

        // if there is no key == array[i] in container linked list
        cur->key = array[i];

        // allocate memory for new node and set it to last node in the linked list
        ContainerNode* node = (ContainerNode*)malloc(sizeof(ContainerNode));
        if (node == NULL) {
            printf("Memory allocation for new ContainerNode failed\n");
            return NULL;
        }

        node->key = NULL;
        cur->next = node;
    }

    return container;
}


// // for testing purposes
// int main() {
//     int size = 9;
//     char *words_array[] = {"ab", "ba", "cd", "dc", "z", "abc", "bca", "bac", "aad"};
//     // init_container(words, words_array, size);
//     Container *words = init_container(words_array, size);
//     ContainerNode* cur;
//     bool has;

//     // visualize hashmap with linked list of nodes
//     for (int i=0; i<size; i++) {
//         printf("%i: ", i);
//         cur = &words->nodes[i];
//         while (cur->key != NULL) {
//             has = contains(cur->key, words);
//             // bool has = 0;
//             printf("%s (%d) -> ", cur->key, has);
//             cur = cur->next;
//         }
//         printf("NULL \n");
//     }

//     // don't have
//     printf("%s = (%d)\n", "ab1c", contains("abc1", words));
//     printf("%s = (%d)\n", "wed", contains("wde", words));
//     printf("%s = (%d)\n", "cab", contains("cab", words));

//     free_container(words);
//     return 0;
// }