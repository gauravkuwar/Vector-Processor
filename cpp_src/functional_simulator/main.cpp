#include <iostream>
#include <filesystem>
#include "core.h"
#include "common.h"
#include "register.h"
#include "parse_asm.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <iodir>\n";
        return 1;
    }

    std::filesystem::path iodir = argv[1];
    FunctionalSimulator fs(iodir);
    fs.run();

    // Call the splitString function
    std::vector<Instruction> instrs = parseAsm(iodir / ASM_CODE_FN);

    for (auto instr : instrs) {
        std::cout << instr.name << " ";
        std::cout << instr.op1 <<  " ";
        std::cout << instr.op2 <<  " ";
        std::cout << instr.op3 <<  " ";
        std::cout << "; " << instr.num_of_ops <<  "\n";

    }

    return 0;
}