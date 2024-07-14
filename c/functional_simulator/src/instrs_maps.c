#include "hashmap.h"

// define array of different instruction types for all instructions
// each item is {name, type_of_ops/num_of ops} 
// V = vector; S = scalar; I = immediate 

char* type_of_ops[][2] = {
    {"ADDVV", "VVV"}, {"SUBVV", "VVV"}, {"MULVV", "VVV"}, {"DIVVV", "VVV"}, // vector vector ops
    {"ADDVS", "VVS"}, {"SUBVS", "VVS"}, {"MULVS", "VVS"}, {"DIVVS", "VVS"}, // vector scalar ops
    {"SEQVV", "VV"}, {"SNEVV", "VV"}, {"SGTVV", "VV"}, {"SLTVV", "VV"}, {"SGEVV", "VV"}, {"SLEVV", "VV"}, // vector vector predicate
    {"SEQVS", "VS"}, {"SNEVS", "VS"}, {"SGTVS", "VS"}, {"SLTVS", "VS"}, {"SGEVS", "VS"}, {"SLEVS", "VS"}, // vector scalar predicate
    {"LVWS", "VSS"}, {"SVWS", "VSS"}, {"LVI", "VSV"}, {"SVI", "VSV"}, {"LV", "VS"}, {"SV", "VS"}, {"LS", "SSI"}, {"SS", "SSI"}, // memory ops
    {"BEQ", "SSI"}, {"BNE", "SSI"}, {"BGT", "SSI"}, {"BLT", "SSI"}, {"BGE", "SSI"}, {"BLE", "SSI"}, // branch instrs
    {"ADD", "SSS"}, {"SUB", "SSS"}, {"AND", "SSS"}, {"OR", "SSS"}, {"XOR", "SSS"}, {"SLL", "SSS"}, {"SRL", "SSS"}, {"SRA", "SSS"}, // scalar scalar ops
    {"POP", "S"}, {"MTCL", "S"}, {"MFCL", "S"}, {"CVM", ""}, {"HALT", ""} // misc
};

char* vector_arithmetic_instrs_array[][2] = {{"UNPACKHI", ""}, {"UNPACKLO", ""}, {"PACKLO", ""}, {"PACKHI", ""}};
char* vector_predicate_instrs_array[][2] = {};
char* memory_instrs_array[][2] = {};
char* scalar_instrs_array [][2]= {};
char* shuffle_instrs_array[][2] = {{"UNPACKHI", ""}, {"UNPACKLO", ""}, {"PACKLO", ""}, {"PACKHI", ""}};;
char* branch_instrs_array[][2] = {};
char* misc_instrs_array[][2] = {"CVM", "POP", "MTCL", "MFCL"};

// initalize hashmaps
Hashmap* vector_arithmetic_instrs;
Hashmap* vector_predicate_instrs;
Hashmap* memory_instrs;
Hashmap* scalar_instrs;
Hashmap* shuffle_instrs;
Hashmap* branch_instrs;
Hashmap* misc_instrs;

void init_instr_maps() {
    vector_arithmetic_instrs = init_map(vector_arithmetic_instrs_array, sizeof(vector_arithmetic_instrs_array) / sizeof(char*));
    vector_predicate_instrs = init_map(vector_predicate_instrs_array, sizeof(vector_predicate_instrs_array) / sizeof(char*));
    memory_instrs = init_map(memory_instrs_array, sizeof(memory_instrs_array) / sizeof(char*));
    scalar_instrs = init_map(scalar_instrs_array, sizeof(scalar_instrs_array) / sizeof(char*));
    shuffle_instrs = init_map(shuffle_instrs_array, sizeof(shuffle_instrs_array) / sizeof(char*));
    branch_instrs = init_map(branch_instrs_array, sizeof(branch_instrs_array) / sizeof(char*));
    misc_instrs = init_map(misc_instrs_array, sizeof(misc_instrs_array) / sizeof(char*));
}

void free_instr_maps() {
    free_map(vector_arithmetic_instrs);
    free_map(vector_predicate_instrs);
    free_map(memory_instrs);
    free_map(scalar_instrs);
    free_map(shuffle_instrs);
    free_map(branch_instrs);
    free_map(misc_instrs);
}