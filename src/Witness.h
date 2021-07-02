//
// Created on 6/11/21.
//

#ifndef _WITNESS_H
#define _WITNESS_H
#include <memory>
#include <vector>

#include "ProtocolElement.h"

class Witness: public ProtocolElement
{
public:
    std::vector<unsigned char> script_;
    int64_t witness_len;
public:
    virtual void unpack_hex(std::stringstream &ss);
    virtual int pack_hex(std::stringstream &ss);

    static std::vector<Witness> unpack_list(std::stringstream &ss);
    static void pack_list(std::stringstream &ss, std::vector<Witness> &wlist);

    virtual bool is_valid();
};


#endif //_WITNESS_H
