#ifndef INSTRS_MAPS_H
#define INSTRS_MAPS_H

#include "hashmap.h"

extern Hashmap* all_instrs;
extern Hashmap* vector_arithmetic_instrs;
extern Hashmap* vector_predicate_instrs;
extern Hashmap* memory_instrs;
extern Hashmap* scalar_instrs;
extern Hashmap* shuffle_instrs;
extern Hashmap* branch_instrs;
extern Hashmap* misc_instrs;

void init_instr_maps();
void free_instr_maps();

#endif // INSTRS_MAPS_H