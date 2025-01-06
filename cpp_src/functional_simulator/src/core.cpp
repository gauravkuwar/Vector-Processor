#include <string>
#include <filesystem>
#include "core.h"
#include "common.h"

#include <iostream>

FunctionalSimulator::FunctionalSimulator(const std::filesystem::path iodir) : 
    SDMEM(iodir / SDMEM_FN, SDMEM_SIZE), 
    VDMEM(iodir / SDMEM_FN, SDMEM_SIZE),
    SREG(SREG_SHAPE),
    VREG(VREG_SHAPE),
    VMASK_REG(VMASK_REG_SHAPE) {
    
    // check if iodir is a valid path
    // filter comments and decode assembly code into struct
    // verify assembly code 
}

void FunctionalSimulator::run() {
    // run
    
    // IF
    // ID
    // EX
    // MEM
    // WB
}