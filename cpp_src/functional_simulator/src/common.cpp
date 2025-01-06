#include "common.h"
#include <string>
#include <vector>
#include <sstream>
#include <regex>

std::string trim(const std::string& str) {
    std::regex rgx("^\\s+|\\s+$");
    return std::regex_replace(str, rgx, "");;
}

std::vector<std::string> splitString(const std::string& str) {
    std::istringstream stream(str);
    std::vector<std::string> parts;
    std::string part;

    while (stream >> part) {
        parts.push_back(part);
    }

    return parts;
}