#include <load.h>
#include "BitcoinVM.h"
#include "opcodes.h"

bool BitcoinVM::execute(std::vector<unsigned char> op_codes)
{
    uint64_t  pc=0;
    while(pc<op_codes.size()-1)
    {
        //TODO: do actual things - just not pretend to do it
        switch(op_codes[pc])
        {
            case opcode::OP_DUP:
            {
                ENSURE_IS_VALID(stack_.size() > 0);
                auto v = stack_.top();
                stack_.push(v);
                pc++;
            }
            break;
            case opcode::OP_HASH160:
            {
                // its 2:29 am and lets cheat for now
                variable v;
                v.type_ = value_type::STRING;
                v.str = "hello";
                stack_.push(v);
                assert(stack_.size() == 4);
                pc++;
            }
            break;
            case opcode::OP_EQUALVERIFY:
            {
                auto v1 = stack_.top();
                stack_.pop();
                auto v2 = stack_.top();
                stack_.pop();
                ENSURE_IS_VALID(v1 == v2);
                pc++;
            }
            break;
            case opcode::OP_CHECKSIG:
            {
                auto v0 = stack_.top();
                stack_.pop();
                // generate signature for v1
                std::vector<unsigned char> sign = {'a', 'b', 'c'};
                variable v2;
                v2.type_ = value_type::DATA;
                v2.data = sign;

                auto v1 = stack_.top();
                stack_.pop();

                variable v;
                v.type_ = value_type::BOOLEAN;
                v.value_.bval = (v1 == v2);
                stack_.push(v);
                pc++;
            }
            break;
            default:
                if (op_codes[pc] >=1 && op_codes[pc] <= 75)
                {
                    uint8_t data_len = op_codes[pc];
                    //TODO: push the data to stack
                    //TODO: and fix the stack - it should already fail
                    pc++;
                    pc+=data_len;
                }
                else
                {
                    return false;
                }
        }
    }

    if(stack_.size() != 1)
    {
        return false;
    }

    auto v = stack_.top();
    if(v.type_ == value_type::BOOLEAN)
    {
        return v.value_.bval;
    }

    return true;
}