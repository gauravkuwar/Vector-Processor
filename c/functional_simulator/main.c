// Functional Simulator
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
// #include "../helpers/set.h"
// #include "instrs_sets.h"

// 32 bit words
#define MAX_VEC_LEN 64
#define REG_COUNT 8
#define VDMEM_SIZE 128000 // 512 KB -> 512000 bytes -> 4096000 bits / 32 bits -> 128000 words
#define SDMEM_SIZE 8000 // 32 KB -> 32000 bytes -> 256000 bits / 32 bits -> 8000 words
#define MAX_INSTR_LINE_LEN 23 // max len of instr name + op1 len + op2 len + op3 len + spaces + extra = 6 + 4 + 4 + 4 + 3 + 2
#define MAX_SUB_STR_LEN 8

// helper functions
void print_usage() {
    printf("usage: ./funcsim [iodir]\n");
}

bool contains(char *val, char *array[], int size) {
    // check if val is in array
    for (int i=0; i<size; i++) {
        if (array[i] == val)
            return true;
    }
    return false;
}

bool str_is_null(char *s) {
    return s[0] == '\0';
}

void str_set_null(char *s) {
    s[0] = '\0';
}

// memory
int32_t vdmem[VDMEM_SIZE];
int32_t sdmem[SDMEM_SIZE];

// registers
int vector_len = MAX_VEC_LEN;
int vector_mask_reg[MAX_VEC_LEN];
int32_t scalar_regs[REG_COUNT];
int32_t vector_regs[REG_COUNT][MAX_VEC_LEN];

void init_vec_mask_reg() {
    // set all bits in vector mask register to 1
    for (int i=0; i<MAX_VEC_LEN; i++)
        vector_mask_reg[i] = 1;
}

// void load_vdmem() {};
// void load_sdmem() {};

// void vvop(int32_t *dv, int32_t *sv1, int32_t *sv2, func) {
//     for (int i=0; i<vector_len; i++)
//         dv[i] = func(sv1[i], sv2[i]);
// }

// void vsop(int32_t *dv, int32_t *sv, int32_t ss, func) {
//     for (int i=0; i<vector_len; i++)
//         dv[i] = func(sv[i], ss);
// }

struct Instr {
    char name[MAX_SUB_STR_LEN];
    char op1[MAX_SUB_STR_LEN];
    char op2[MAX_SUB_STR_LEN];
    char op3[MAX_SUB_STR_LEN];
} Instr;


int get_num_of_instrs(FILE *asm_file) {
    int num_of_instrs = 0;
    char buf[1024];
        
    // get number of instructions in file
    while (fgets(buf, sizeof(buf), asm_file) != NULL) {
        // Check if the line is not empty and not a comment
        if ((buf[0] != '\n' && buf[0] != '\0' && buf[0] != '#'))
            num_of_instrs++;
    }
    fseek(asm_file, 0, SEEK_SET);
    // printf("Num of instr = %d\n", num_of_instrs);
    return num_of_instrs;
}


int fetch_decode(char *iodir) {
    const char *code_asm = "/Code.asm";
    char asm_fp[strlen(iodir) + strlen(code_asm)];

    char buf[1024];
    int num_of_instrs = 0;

    char char_buf;
    char str_buf[MAX_SUB_STR_LEN];
    
    int cidx = 0; // counter for character for str_buf
    int sidx = 0; // counter keep track of substrings
    int iidx = 0; // counter for instruction for instrs

    // Derive asm file path
    strcpy(asm_fp, iodir);
    strcat(asm_fp, "/Code.asm");

    FILE *asm_file = fopen(asm_fp, "r");
    if (asm_file == NULL) {
        perror("Error opening Code.asm file");
        return -1;
    }
    
    // get number of instructions in file
    while (fgets(buf, sizeof(buf), asm_file) != NULL) {
        // Check if the line is not empty and not a comment
        if ((buf[0] != '\n' && buf[0] != '\0' && buf[0] != '#'))
            num_of_instrs++;
    }
    fseek(asm_file, 0, SEEK_SET);
    printf("Num of instr = %d\n", num_of_instrs);
    
    
    // Read Code.asm, filter out comments and store as Instr struct in instrs array
    struct Instr instrs[num_of_instrs]; // array of instructions
    // initialize all strings in struct to null
    for (int i=0; i<num_of_instrs; i++) {
        str_set_null(instrs[i].name);
        str_set_null(instrs[i].op1);
        str_set_null(instrs[i].op2);
        str_set_null(instrs[i].op3);
    }

    // The loop decodes instruction and splits them into name and operands
    // and stores in instrs array
    do {
        char_buf = fgetc(asm_file);
        if (char_buf == '#') // skip that line because its comment
            sidx = 4;
        
        if (sidx < 4) {
            if (char_buf == ' ' || char_buf == '\n' || char_buf == EOF) {                
                if (iidx >= num_of_instrs) {
                    fprintf(stderr, "Error: iidx exceeds num_of_instrs\n");
                    return -1;
                }

                str_buf[cidx] = '\0'; // add NULL at end
                if (sidx == 0)
                    strcpy(instrs[iidx].name, str_buf);
                else if (sidx == 1)
                    strcpy(instrs[iidx].op1, str_buf);
                else if (sidx == 2)
                    strcpy(instrs[iidx].op2, str_buf);
                else if (sidx == 3)
                    strcpy(instrs[iidx].op3, str_buf);
    
                str_set_null(str_buf);
                cidx = 0;
                sidx++;
            } else {
                str_buf[cidx] = char_buf;
                cidx++;
            }
        }
        
        if (char_buf == '\n' || char_buf == EOF) {
            if (!str_is_null(instrs[iidx].name)) {
                // verify if instruction is valid else return error
                // check if its in set of all instructions
                // if (instrs[iidx].name not in set_of_all_instrs) {
                //     return -1
                // }
                iidx++;
            }

            cidx = 0;
            sidx = 0;
        }
    } while (char_buf != EOF);

    // printf("%s, %s, %s, %s\n", instrs[iidx].name, instrs[iidx].op1, instrs[iidx].op2, instrs[iidx].op3);

    fclose(asm_file);
    return 0;
}

int main(int argc, char *argv[]) {
    // handle help option
    for (int i=1; i<argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        }
    }

    // check if there is iodir arg
    if (argc < 2) {
        printf("Error: No iodir specificed\n");
        print_usage();
        return 1;
    }


    char *iodir = argv[1];
    const char *code_asm = "/Code.asm";
    char asm_fp[strlen(iodir) + strlen(code_asm)];

    // Derive asm file path
    strcpy(asm_fp, iodir);
    strcat(asm_fp, "/Code.asm");

    FILE *asm_file = fopen(asm_fp, "r");
    if (asm_file == NULL) {
        perror("Error opening Code.asm file");
        return -1;
    }

    int num_of_instrs = get_num_of_instrs(asm_file);
    fetch_decode(iodir);



    // char str[] = "INST OP1 OP2 # comment";
    // const char delim[] = " "; // Delimiter

    // // Use strtok to split the string
    // char *token = strtok(str, " ");
    // char *substrs[4];
    // int idx = 0;

    // // Loop through the tokens
    // while (idx < 4 && token != NULL) {
    //     // printf("%s\n", token);
    //     substrs[idx++] = token;
    //     token = strtok(NULL, delim);
    // }

    // for (int i=0; i<4; i++)
    //     printf("%s\n", substrs[i]); 

    // TODO: check if iodir is valid path

    // int num_of_instrs;
    // char *all_instrs[num_of_instrs];

    // // fetch and verify instrs from assembly file
    // // decode instrs

    // init_vec_mask_reg();
    // int PC = 0;

    // while (strcmp(all_instrs[PC], "HALT") != 0) {
    //     instr_name, op1, op2, op3 = all_instrs[PC];

    //     if instr_name in vector_arithmetic or vector_predicates:
    //         ...
    //     if instr_name in memory_op:
    //         ...
    //     if instr_name in scalar_ops:
    //         ...
    //     if instr_name in misc:
    //         ...
    //     if instr_name in branch:
    //         ...
    // }

    return 0;
}