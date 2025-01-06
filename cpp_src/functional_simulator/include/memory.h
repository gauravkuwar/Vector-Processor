#ifndef MEMORY_H
#define MEMORY_H
#include <string>

class Memory {
private:
    int32_t* data;
    int size;
public:
    Memory(const std::filesystem::path fp, int size);
    int32_t read(int32_t addr);
    void write(int32_t addr, int32_t value);
    void dump(const std::filesystem::path fp);
    ~Memory();
};

#endif
