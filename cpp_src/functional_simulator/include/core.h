#ifndef CORE_H
#define CORE_H
#include <string>
#include "memory.h"
#include "register.h"

class FunctionalSimulator {
private:
    Memory SDMEM, VDMEM;
    Register SREG, VREG, VMASK_REG;
public:
    FunctionalSimulator(const std::filesystem::path iodir);
    void run();
    void IF();
    void ID();
    void EX();
    void MEM();
    void WB();

};

#endif
