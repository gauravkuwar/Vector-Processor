#include <fstream>
#include <regex>
#include "common.h"

bool isCommentOrEmpty(std::string& line) {
    return line.empty() || line[0] == '#';
}


std::string removeInlineComments(std::string& line) {
    std::regex rgx("\\s*#.*$");
    return std::regex_replace(line, rgx, "");
}


// decode assembly code into struct
Instruction str2Struct(std::string& line) {
    std::vector<std::string> parts = splitString(line);

    Instruction instr;
    instr.name = parts[0];
    instr.op1 = parts[1];
    instr.op2 = parts[2];
    instr.op3 = parts[3];
    instr.num_of_ops = (int)parts.size() - 1;

    return instr;
}

// get code from asm file and put lines in an vector
std::vector<Instruction> parseAsm(const std::filesystem::path fp) {
    std::ifstream file(fp);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + fp.string());  // Throw exception on error
    }

    std::string line;
    std::string trimmed_line;
    std::string comment_rmed;
    std::vector<Instruction> instrs;

    // Read the file line by line
    while (std::getline(file, line)) {
        trimmed_line = trim(line);
        if (!isCommentOrEmpty(trimmed_line)) {
            comment_rmed = removeInlineComments(trimmed_line);
            instrs.push_back(str2Struct(comment_rmed));
        }
    }

    return instrs;
}