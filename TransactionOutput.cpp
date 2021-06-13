#include <memory>
#include <cstdint>
#include <cstring>

#include "TransactionOutput.h"
#include "load.h"

TransactionOutput::TransactionOutput()
{
}

TransactionOutput::~TransactionOutput()
{
}

void TransactionOutput::unpack(std::stringstream &ss)
{
    read(ss, 8, &value);
    script_size = read_var_int(ss);
    script = (unsigned char *) malloc(script_size);
    read(ss, script_size, script);
}

int TransactionOutput::pack(char *output, int &output_len)
{
    char *pos = output;
    std::memcpy(pos, &value, sizeof(uint64_t));
    pos += sizeof(uint64_t);
    output_len -= sizeof(uint64_t);

    pos += pack_var_int((unsigned char *) pos, output_len, script_size);

    std::memcpy(pos, script, script_size);
    pos += script_size;
    output_len -= script_size;

    return int(pos - output);
}

int TransactionOutput::pack(std::stringstream &ss)
{
    int pos = 0;
    pos += pack_hex(ss, &value, sizeof(uint64_t));
    pos += pack_var_int(ss, script_size);
    pos += pack_hex(ss, (unsigned char *) script, script_size);
    return pos;
}
