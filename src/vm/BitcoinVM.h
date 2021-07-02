#ifndef BITCOIN_BITCOINVM_H
#define BITCOIN_BITCOINVM_H

#include <stack>
#include "variable.h"

class BitcoinVM
{
public:
    std::stack<variable> stack_;

    virtual bool execute(std::vector<unsigned char> op_code);

};


#endif //BITCOIN_BITCOINVM_H
