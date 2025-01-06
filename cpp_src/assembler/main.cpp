/*
Takes assembly file as input and outputs 
binary encoding of the assembly code
*/

#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>
#include <map>

std::map<std::string, std::map<std::string, int>> op_map = {
    {
        "ADDVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000000}
        },
    },
    {
        "SUBVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000001}
        },
    },
    {
        "MULVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000010}
        },
    },
    {
        "DIVVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000011}
        },
    },
    {
        "ADDVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000000}
        },
    },
    {
        "SUBVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000001}
        },
    },
    {
        "MULVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000010}
        },
    },
    {
        "DIVVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000011}
        },
    },
    {
        "SEQVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000100}
        },
    },
    {
        "SNEVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000101}
        },
    },
    {
        "SGTVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000110}
        },
    },
    {
        "SLTVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b000111}
        },
    },
    {
        "SGEVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b001000}
        },
    },
    {
        "SLEVV", {
            {"opcode", 0b100000}, 
            {"funct6", 0b001001}
        },
    },
    {
        "SEQVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000100}
        },
    },
    {
        "SNEVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000101}
        },
    },
    {
        "SGTVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000110}
        },
    },
    {
        "SLTVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b000111}
        },
    },
    {
        "SGEVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b001000}
        },
    },
    {
        "SLEVS", {
            {"opcode", 0b100001}, 
            {"funct6", 0b001001}
        },
    },
    {
        "CVM", {
            {"opcode", 0b000001}, 
            {"funct6", 0b000000}
        },
    },
    {
        "POP", {
            {"opcode", 0b000010}, 
            {"funct6", 0b000001}
        },
    },
    {
        "MTCL", {
            {"opcode", 0b000011}, 
            {"funct6", 0b000010}
        },
    },
    {
        "MFCL", {
            {"opcode", 0b000100}, 
            {"funct6", 0b000011}
        },
    },
    {
        "LV", {
            {"opcode", 0b100010}, 
            {"funct6", 0b000000}
        },
    },
    {
        "SV", {
            {"opcode", 0b100011}, 
            {"funct6", 0b000000}
        },
    },
    {
        "LVWS", {
            {"opcode", 0b100100}, 
            {"funct6", 0b000000}
        },
    },
    {
        "SVWS", {
            {"opcode", 0b100101}, 
            {"funct6", 0b000000}
        },
    },
    {
        "LVI", {
            {"opcode", 0b100110}, 
            {"funct6", 0b000000}
        },
    },
    {
        "SVI", {
            {"opcode", 0b100111}, 
            {"funct6", 0b000000}
        },
    },
    {
        "LS", {
            {"opcode", 0b000101}, 
        },
    },
    {
        "SS", {
            {"opcode", 0b000110}, 
        },
    },
    {
        "ADD", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000000}
        },
    },
    {
        "SUB", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000001}
        },
    },
    {
        "AND", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000010}
        },
    },
    {
        "OR", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000011}
        },
    },
    {
        "XOR", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000100}
        },
    },
    {
        "SLL", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000101}
        },
    },
    {
        "SRL", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000110}
        },
    },
    {
        "SRA", {
            {"opcode", 0b000000}, 
            {"funct6", 0b000111}
        },
    },
    {
        "BEQ", {
            {"opcode", 0b001000}, 
        },
    },
    {
        "BNE", {
            {"opcode", 0b001001}, 
        },
    },
    {
        "BGT", {
            {"opcode", 0b001010}, 
        },
    },
    {
        "BLT", {
            {"opcode", 0b001011}, 
        },
    },
    {
        "BGE", {
            {"opcode", 0b001100}, 
        },
    },
    {
        "BLE", {
            {"opcode", 0b001101}, 
        },
    },
    {
        "UNPACKLO", {
            {"opcode", 0b100000}, 
            {"funct6", 0b001010}
        },
    },
    {
        "UNPACKHI", {
            {"opcode", 0b100000}, 
            {"funct6", 0b001011}
        },
    },
    {
        "PACKLO", {
            {"opcode", 0b100000}, 
            {"funct6", 0b001100}
        },
    },
    {
        "PACKHI", {
            {"opcode", 0b100000}, 
            {"funct6", 0b001101}
        },
    },
    {
        "HALT", {
            {"opcode", 0b111111}, 
            {"funct6", 0b111111}
        },
    },
};

std::map<std::string, std::string> instr_type_map = {
    {"ADDVV", "R"},
    {"SUBVV", "R"},
    {"MULVV", "R"},
    {"DIVVV", "R"},
    {"ADDVS", "R"},
    {"SUBVS", "R"},
    {"MULVS", "R"},
    {"DIVVS", "R"},
    {"SEQVV", "R"},
    {"SNEVV", "R"},
    {"SGTVV", "R"},
    {"SLTVV", "R"},
    {"SGEVV", "R"},
    {"SLEVV", "R"},
    {"SEQVS", "R"},
    {"SNEVS", "R"},
    {"SGTVS", "R"},
    {"SLTVS", "R"},
    {"SGEVS", "R"},
    {"SLEVS", "R"},
    {"CVM", "R"},
    {"POP", "R"},
    {"MTCL", "R"},
    {"MFCL", "R"},
    {"LV", "R"},
    {"SV", "R"},
    {"LVWS", "R"},
    {"SVWS", "R"},
    {"LVI", "R"},
    {"SVI", "R"},
    {"LS", "I"},
    {"SS", "I"},
    {"ADD", "R"},
    {"SUB", "R"},
    {"AND", "R"},
    {"OR", "R"},
    {"XOR", "R"},
    {"SLL", "R"},
    {"SRL", "R"},
    {"SRA", "R"},
    {"BEQ", "I"},
    {"BNE", "I"},
    {"BGT", "I"},
    {"BLT", "I"},
    {"BGE", "I"},
    {"BLE", "I"},
    {"UNPACKLO", "R"},
    {"UNPACKHI", "R"},
    {"PACKLO", "R"},
    {"PACKHI", "R"},
    {"HALT", "R"},
};

// Helper functions
// Remove leading and trailing whitespace
std::string trim(const std::string& str) {
    std::regex rgx("^\\s+|\\s+$");
    return std::regex_replace(str, rgx, "");;
}

// Split string into vector of strings
std::vector<std::string> splitString(const std::string& str) {
    std::istringstream stream(str);
    std::vector<std::string> parts;
    std::string part;

    while (stream >> part) {
        parts.push_back(part);
    }

    return parts;
}

bool isCommentOrEmpty(std::string& line) {
    return line.empty() || line[0] == '#';
}

std::string removeInlineComments(std::string& line) {
    std::regex rgx("\\s*#.*$");
    return std::regex_replace(line, rgx, "");
}

int get_reg_num(std::string& reg_str) {
    // Register encoding:
    // VR0 -> 10000, and SR0 -> 00000
    // VR1 -> 10001, and SR0 -> 00001
    // ...

    if (reg_str == "")
        return 0;

    int reg_num = std::stoi(reg_str.substr(2));

    if (reg_str[0] == 'V')
        return reg_num + 0b10000;
    else if (reg_str[0] == 'S')
        return reg_num;

    return 0;
}

int main(int argc, char* argv[]) {
    // get filepath from argument
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <assembly_file>\n";
        return 1;
    }

    std::filesystem::path asm_fp = argv[1];
    std::filesystem::path bin_fp = asm_fp;
    bin_fp.replace_extension(".bin");

    std::ifstream asm_file(asm_fp);
    std::ofstream bin_file(bin_fp, std::ios::binary);
    
    if (!asm_file.is_open()) {
        throw std::runtime_error("Error opening input file: " + asm_fp.string());
    }
    if (!bin_file.is_open()) {
        throw std::runtime_error("Error opening output file: " + bin_fp.string());
    }

    std::string line;
    std::string trimmed_line;
    std::string comment_rmed;

    // Define a 32-bit instruction
    uint32_t encoded_instr = 0;

    // Read the file line by line
    while (std::getline(asm_file, line)) {
        trimmed_line = trim(line);

        // Ignore empty lines and comments
        if (isCommentOrEmpty(trimmed_line))
            continue;

        comment_rmed = removeInlineComments(trimmed_line);
        std::vector<std::string> parts = splitString(comment_rmed);

        std::string instr_name = parts[0];
        std::string instr_type = instr_type_map[instr_name];
        int opcode = op_map[instr_name]["opcode"];
        int funct6 = op_map[instr_name]["funct6"];
        int shift5 = 0x00000;

        std::cout << parts[0] << " " << parts[1] << " " << parts[2] << " " << parts[3] << std::endl;
        std::cout << "Instruction Type: " << instr_type << std::endl;
        std::cout << "Opcode (6 bits): " << std::bitset<6>(opcode) << " (" << opcode << ")" << std::endl;
        std::cout << "Funct6 (6 bits): " << std::bitset<6>(funct6) << " (" << funct6 << ")" << std::endl;
        std::cout << "Shift5 (5 bits): " << std::bitset<5>(shift5) << " (" << shift5 << ")" << std::endl;
                
        if (instr_name == "HALT") {
            encoded_instr = 0xFFFFFFFF;
        }
        else if (instr_type == "R") {
            // R-type encoding:
            // opcode (6 bits) | rs (5 bits) | rt (5 bits) | rd (5 bits) | shamt (5 bits) | funct6 (6 bits)

            int read_reg_1 = get_reg_num(parts[2]);
            int read_reg_2 = get_reg_num(parts[3]);
            int write_reg = get_reg_num(parts[1]);
            
            // special case for S__VV, S__VS, and MTCL
            if (instr_name.size() == 5 && instr_name.substr(0,1) == "S" && (instr_name.substr(3,2) == "VV" || instr_name.substr(3,2) == "VS")) {
                // Since, S__VV and S__VS write to vector mask register, there is no write register
                // So, the first register is the read register 1 and the second register is the read register 2
                read_reg_2 = read_reg_1;
                read_reg_1 = write_reg;
                write_reg = 0;
            }
            else if (instr_name == "MTCL") {
                // Since, MTCL writes to the vector length register, there is no write register
                // So, the first and only register is the read register 1 which is read from
                read_reg_1 = write_reg;
                write_reg = 0;
            }

            encoded_instr = (opcode << 26) | (read_reg_1 << 20) | (read_reg_2 << 15) | (write_reg << 10) | (shift5 << 5) | (funct6 << 0);

            // Print binary representation and integer values of instruction fields
            std::cout << "Read reg 1 (5 bits): " << std::bitset<5>(read_reg_1) << " (" << read_reg_1 << ")" << std::endl;
            std::cout << "Read reg 2 (5 bits): " << std::bitset<5>(read_reg_2) << " (" << read_reg_2 << ")" << std::endl;
            std::cout << "Write reg (5 bits): " << std::bitset<5>(write_reg) << " (" << write_reg << ")" << std::endl;
        }
        else if (instr_type == "I") {
            // I-type encoding:
            // opcode (6 bits) | rs (5 bits) | rt (5 bits) | immediate (16 bits)

            int read_reg_1 = get_reg_num(parts[2]);
            int write_reg = get_reg_num(parts[1]);
            int imm = std::stoi(parts[3]);
            
            // Handle negative immediate values by masking to 16 bits
            if (imm < 0) 
                imm = imm & 0xFFFF;

            encoded_instr = (opcode << 26) | (read_reg_1 << 20) | (write_reg << 15) | (imm << 0);

            // Print binary representation and integer values of instruction fields
            std::cout << "Read reg 1 (5 bits): " << std::bitset<5>(read_reg_1) << " (" << read_reg_1 << ")" << std::endl;
            std::cout << "Write reg (5 bits): " << std::bitset<5>(write_reg) << " (" << write_reg << ")" << std::endl;
            std::cout << "Immediate (16 bits): " << std::bitset<16>(imm) << " (" << imm << ")" << std::endl;
            
        }

        std::cout << "Encoded instruction: " << std::bitset<32>(encoded_instr) << " (" << encoded_instr << ")" << std::endl;
        // Write encoded instruction to binary file
        bin_file.write(reinterpret_cast<char*>(&encoded_instr), sizeof(encoded_instr));
        std::cout << std::endl;
    }

    // Close files
    asm_file.close();
    bin_file.close();
    return 0;
}