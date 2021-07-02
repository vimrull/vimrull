#include <memory>
#include <cstdint>
#include <cstring>

#include "Transaction.h"
#include "load.h"

// Witness upgrade activated at block 477,120
static const int SERIALIZE_TRANSACTION_NO_WITNESS = 0x40000000;

const char POSITIVE_WITNESS_FLAG[2] = { 0, 1};

Transaction::Transaction()
{
}

Transaction::~Transaction()
{
}

void Transaction::unpack_hex(std::stringstream &ss)
{
    version = read_int32(ss);

    if (allow_witness())
    {
        std::tie(has_witness, input_count) = readWitnessAndInputCount(ss);
    }

    for (int i = 0; i < input_count; i++)
    {
        TransactionInput input;
        input.unpack_hex(ss);
        inputs.push_back(input);
    }

    output_count = read_var_int(ss);

    for (int i = 0; i < output_count; i++)
    {
        TransactionOutput output;
        output.unpack_hex(ss);
        outputs.push_back(output);
    }

    //std::cout << "After output: " << ss.tellg() << std::endl;

    if (has_witness)
    {
        for (auto& inp: inputs)
        {
            inp.witness_list = Witness::unpack_list(ss);
        }
    }

    //std::cout << "After witness: " << ss.tellg() << std::endl;
    locktime = read_int32(ss);
}

int Transaction::pack_hex(std::stringstream &ss)
{
    int pos = 0;

    pos += pack_ptr2hex(ss, &version, sizeof(int32_t));

    if(has_witness)
    {
        pos += pack_ptr2hex(ss, (void *) POSITIVE_WITNESS_FLAG, 2);
    }

    pos += pack_var_int(ss, input_count);

    for (varint i = 0; i < input_count; i++)
    {
        auto txi = inputs[i];
        pos += txi.pack_hex(ss);
    }

    pos += pack_var_int(ss, output_count);

    for (varint i = 0; i < output_count; i++)
    {
        auto txo = outputs[i];
        pos += txo.pack_hex(ss);
    }

    //std::cout << "After output: " << ss.tellp() << std::endl;

    if (has_witness)
    {
        for (auto& inp: inputs)
        {
            Witness::pack_list(ss, inp.witness_list);
        }
    }

    //std::cout << "After witness: " << ss.tellp() << std::endl;

    pos += pack_ptr2hex(ss, &locktime, sizeof(uint32_t));
    return pos;
}

bool Transaction::allow_witness()
{
    return !(version & SERIALIZE_TRANSACTION_NO_WITNESS);
}

void Transaction::print()
{
    std::cout << "------------" << std::endl;
    std::cout << "Version: " << version << "\nHas witness: " << has_witness << "\nInput count: " << input_count
              << std::endl;
    std::cout << "------------" << std::endl;
    for (int i = 0; i < input_count; i++)
    {
        auto input = inputs[i];

        printf("Hash:");
        hexdump((unsigned char *) input.previous_output.hash, 32);
        std::cout << "Input index: " << std::hex << input.previous_output.index << "\nScript size: "
                  << input.script_size << std::endl;
        if (input.IsCoinbase())
        {
            std::cout << "Coinbase Tx. Block height = 0x" << input.block_height << std::endl;
        }
        std::cout << "Sequence:" << input.sequence << std::endl;
    }
    std::cout << "------------" << std::endl;
    std::cout << "Output count: " << output_count << std::endl;

    for (int o = 0; o < output_count; o++)
    {
        auto output = outputs[o];
        std::cout << "Value: " << std::dec << output.value << std::endl;
        std::cout << "Script size: " << std::dec << output.script_size << std::endl << "Script: ";
        hexdump(output.script, output.script_size);
    }

    std::cout << "Lock time: " << std::dec << locktime << std::endl;
}

//https://en.bitcoin.it/wiki/Protocol_rules
bool Transaction::is_valid()
{
    // Make sure neither in or out lists are empty
    ENSURE_IS_VALID(input_count > 0 && output_count > 0);
    ENSURE_IS_VALID(inputs.size() == input_count);
    ENSURE_IS_VALID(outputs.size() == output_count);

    for(auto& inp: inputs)
    {
        ENSURE_IS_VALID(inp.is_valid());
    }

    for(auto& outp: outputs)
    {
        ENSURE_IS_VALID(outp.is_valid());
    }

    return true;
}

bool Transaction::pack_hex_for_validation(std::stringstream ss, std::vector<unsigned char> locking_script)
{
    return false;
}
