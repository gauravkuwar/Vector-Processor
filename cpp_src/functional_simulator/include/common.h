#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>
#include <filesystem>

enum OPERAND_TYPE {VECTOR, SCALAR, IMM};
enum INSTRUCTION {PACKLO, SUBVV, MULVV, ADDVV, PACKHI, UNPACKHI, UNPACKLO, DIVVV, DIVVS, MULVS, SUBVS, ADDVS, SLEVV, SGTVV, SEQVV, SLTVV, SGEVV, SNEVV, SGTVS, SLEVS, LV, SNEVS, SLTVS, SEQVS, SV, SGEVS, POP, MFCL, MTCL, SVWS, LVWS, LVI, SVI, BGT, BLE, BLT, LS, BGE, BNE, SS, BEQ, SUB, OR, AND, SRA, ADD, SRL, SLL, XOR, CVM, HALT};

struct Operand {
    REG_TYPE reg_type;
    int idx;
}

struct Instruction {
    std::string name;
    Operand op1;
    Operand op2;
    Operand op3;
    int num_of_ops;
};

// Note: All data is 32 bit (except vector mask reg)

// Memory is word addressable
const int VDMEM_SIZE = 128000; // 512 KB -> 4096000 bits / 32 bit = 128000 words
const int SDMEM_SIZE = 8000; // 32 KB -> 256000 bits / 32 bit = 8000 words

const int SREG_SHAPE[2] = {8, 1};
const int VREG_SHAPE[2] = {8, 64};
const int VLEN_REG_SHAPE[2] = {1, 1};
const int VMASK_REG_SHAPE[2] = {1, 64};

// Filenames
const std::filesystem::path ASM_CODE_FN = "Code.asm";

const std::filesystem::path SDMEM_FN = "SDMEM.txt";
const std::filesystem::path VDMEM_FN = "VDMEM.txt";

const std::filesystem::path SDMEM_OP_FN = "SDMEMOP.txt";
const std::filesystem::path VDMEM_OP_FN = "VDMEMOP.txt";

const std::filesystem::path VRF_OP_FN = "VRF.txt";
const std::filesystem::path SRF_OP_FN = "SRF.txt";

std::string trim(const std::string& str);
std::vector<std::string> splitString(const std::string& str);

#endif
