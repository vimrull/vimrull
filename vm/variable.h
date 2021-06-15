#ifndef BITCOIN_VARIABLE_H
#define BITCOIN_VARIABLE_H

#include <cstdint>
#include <string>
#include <vector>

enum value_type
{
    BOOLEAN,
    CHAR,
    INT8,
    INT16,
    INT32,
    INT64,
    BYTE,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    STRING,
    DATA
};

union value {
    bool bval;
    char ch;
    int8_t int8;
    int16_t int16;
    int32_t int32;
    int64_t int64;
    unsigned char byte;
    uint8_t uint8;
    uint16_t uint16;
    uint32_t uint32;
    uint64_t uint64;
};

class variable
{
public:
    value_type type_;
    value value_;
    std::string str;
    std::vector<unsigned char> data;
    variable() {}
    variable(value_type typ, value val);
    variable(std::string str);
    variable(std::vector<unsigned char> data);
    variable(const variable &var);

    bool operator==(const variable& other);
};

#endif //BITCOIN_VARIABLE_H
