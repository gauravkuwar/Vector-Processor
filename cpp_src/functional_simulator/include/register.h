#ifndef REGISTER_H
#define REGISTER_H
#include <string>

class Register {
private:
    int32_t** data;
    int shape[2];
public:
    Register(const int shape[2]);
    int32_t read(int reg_num, int idx);
    void write(int reg_num, int idx, int32_t value);
    void dump(const std::filesystem::path fp);
    ~Register();
};

#endif
