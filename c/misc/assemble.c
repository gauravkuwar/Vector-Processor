#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // get arguments, both about the same
    char *asm_fn = NULL;
    char *vdmem_fn = NULL;
    char *sdmem_fn = NULL;

    for (int i=1; i<argc; i++) {
        if (strcmp(argv[i], "--asm") == 0)
            asm_fn = argv[++i];
        
        if (strcmp(argv[i], "--vdmem") == 0)
            vdmem_fn = argv[++i];

        if (strcmp(argv[i], "--sdmem") == 0)
            sdmem_fn = argv[++i];
    }

    printf("asm_fn = %s\n", asm_fn);
    printf("vdmem_fn = %s\n", vdmem_fn);
    printf("sdmem_fn = %s\n", sdmem_fn);

    // convert relative paths to absolule paths

    // string splitting
    // both seem the same, but C++ is more robust

    if (vdmem_fn != NULL) {
        // read file

        // convert each line (int) to binary 32-bit
        // C++ is much less code

        // write to file (binary)

        // hashmaps are easily supported in C++,
        // while C we have to make our own hashmap functions
    }

    return 0;
}