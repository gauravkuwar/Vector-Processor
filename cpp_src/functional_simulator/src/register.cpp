#include <fstream>
#include <iostream>
#include "register.h"

Register::Register(const int shape[2]) {
    this->data = new int32_t*[shape[0]];
    for (int i=0; i<shape[0]; i++) {
        this->data[i] = new int32_t[shape[1]];
    }

    this->shape[0] = shape[0];
    this->shape[1] = shape[1];
};

int32_t Register::read(int reg_num, int idx) {
    return this->data[reg_num][idx];
};

void Register::write(int reg_num, int idx, int value) {
    this->data[reg_num][idx] = value;
};

void Register::dump(const std::filesystem::path fp) {
    // Open the file for writing (it will overwrite if it exists)
    std::ofstream outFile(fp);

    if (!outFile.is_open()) {
        throw std::runtime_error("Error opening file: " + fp.string());  // Throw exception on error
    }

    const int MAX_COL_LEN = 13;
    std::string tmp;

    // Write the data contents to the file
    // Add reg nums
    for (int i=0; i<shape[0]; i++) {
        tmp = std::to_string(i);
        tmp.resize(MAX_COL_LEN, ' ');
        outFile << tmp;
    }

    outFile << std::endl;

    // add seperating line
    std::string sep_line(shape[0] * MAX_COL_LEN, '-');
    outFile << sep_line << std::endl;

    // Add reg values
    for (int j=0; j<shape[1]; j++) {
        for (int i=0; i<shape[0]; i++) {
            tmp = std::to_string(this->data[i][j]);
            tmp.resize(MAX_COL_LEN, ' ');
            outFile << tmp;
        }
        outFile << std::endl;
    }
    
    // Close the file
    outFile.close();
};

Register::~Register() {
    for (int i=0; i<this->shape[0]; i++) {
        delete[] this->data[i];
    }
    delete[] this->data;
};