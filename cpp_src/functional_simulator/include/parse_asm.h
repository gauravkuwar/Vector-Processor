#ifndef PARSE_ASM_H
#define PARSE_ASM_H
#include <string>
#include <vector>
#include "common.h"

bool isCommentOrEmpty(std::string& line);
Instruction str2Struct(std::string& line);
std::vector<Instruction> parseAsm(const std::filesystem::path fp);

#endif
