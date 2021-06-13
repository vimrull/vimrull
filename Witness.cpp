//
// Created on 6/11/21.
//

#include "Witness.h"
#include "load.h"

void Witness::unpack_hex(std::stringstream &ss)
{
    witness_len = read_var_int(ss);
    if (witness_len > 0)
    {
        script_.reserve(witness_len);
        script_.resize(witness_len);
        read(ss, witness_len, (char *) script_.data());
        int len = script_.size();
        assert(len == witness_len);
    }
}

int Witness::pack_hex(std::stringstream &ss)
{
    pack_var_int(ss, witness_len);
    if (witness_len > 0)
    {
        pack_ptr2hex(ss, script_.data(), witness_len);
    }
}

std::vector<Witness> Witness::unpack_list(std::stringstream &ss)
{
    std::vector<Witness> witness_list;
    int64_t witness_count = read_var_int(ss);
    for(int i=0;i<witness_count;i++)
    {
        Witness witness;
        witness.unpack_hex(ss);
        witness_list.push_back(witness);
    }

    return witness_list;
}

void Witness::pack_list(std::stringstream &ss, std::vector<Witness> &wlist)
{
    pack_var_int(ss, wlist.size());
    for(auto& w: wlist)
    {
        w.pack_hex(ss);
    }
}

bool Witness::is_valid()
{
    return true;
}