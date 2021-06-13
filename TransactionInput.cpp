#include <memory>
#include <cstdint>
#include <cstring>

#include "TransactionInput.h"
#include "load.h"


TransactionInput::TransactionInput()
{
    witness_count = 0;
}

TransactionInput::~TransactionInput()
{
}

void TransactionInput::unpack(std::stringstream &ss)
{
    previous_output.unpack(ss);
    //read(ss, 32, previous_output.hash);
    //previous_output.index = read_int32(ss);
    script_size = read_var_int(ss);
    if (script_size > 0)
    {
        script = (unsigned char *) malloc(script_size);
        read(ss, script_size, script);
    }
    sequence = read_int32(ss);

    if (IsCoinbase())
    {
        ReadBlockHeight();
    }
}

void TransactionInput::ReadBlockHeight()
{
    if (!IsCoinbase())
    {
        return;
    }

    char cbhl = script[0];
    //assert(cbhl == 3);
    if (cbhl == 3)
    {
        char data[4];
        data[0] = 0;
        block_height = 0; // since we are updating 3 bytes lets clear first
        std::memcpy(&block_height, &script[1], 3);
    } else
    {
        block_height = 0;
    }
}

int TransactionInput::pack(char *output, int &output_len)
{
    char *pos = output;
    pos += previous_output.pack(pos, output_len);

    pos += pack_var_int((unsigned char *) pos, output_len, script_size);

    auto code_size = script_size;
    std::memcpy(pos, script, code_size);
    pos += code_size;
    output_len -= code_size;

    std::memcpy(pos, &sequence, sizeof(int32_t));
    pos += sizeof(int32_t);
    output_len -= sizeof(int32_t);

    return int(pos - output);
}

int TransactionInput::pack(std::stringstream &ss)
{
    int pos = 0;
    pos += previous_output.pack(ss);
    pos += pack_var_int(ss, script_size);
    if (script_size > 0)
    {
        pos += pack_hex(ss, script, script_size);
    }

    pos += pack_hex(ss, &sequence, sizeof(int32_t));
    return pos;
}

bool TransactionInput::IsCoinbase()
{
    return previous_output.index == 0xFFFFFFFF;
}
