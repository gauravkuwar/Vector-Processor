#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "memory.h" 

// BASE MEMORY CLASS

Memory::Memory(const std::filesystem::path fp, int size) {
    this->size = size;
    this->data = new int32_t[size];
    std::ifstream file(fp);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + fp.string());  // Throw exception on error
    }

    std::string line;
    int32_t addr = 0;

    // Read the file line by line
    while (std::getline(file, line)) {
        this->data[addr] = std::stoi(line);
        addr++;
    }

    // Close the file
    file.close();
};

int32_t Memory::read(int32_t addr) {
    return this->data[addr];
};

void Memory::write(int32_t addr, int32_t value) {
    this->data[addr] = value;
};

void Memory::dump(const std::filesystem::path fp) {
    // Open the file for writing (it will overwrite if it exists)
    std::ofstream outFile(fp);

    if (!outFile.is_open()) {
        throw std::runtime_error("Error opening file: " + fp.string());  // Throw exception on error
    }

    // Write the array contents to the file
    for (int32_t addr=0; addr<this->size; addr++) {
        outFile << this->data[addr] << std::endl;
    }
    // Close the file
    outFile.close();
};

Memory::~Memory() {
    delete[] this->data;
};