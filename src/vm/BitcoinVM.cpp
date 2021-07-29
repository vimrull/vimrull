#include <load.h>
#include <crypto/util.h>
#include "BitcoinVM.h"
#include "opcodes.h"

bool BitcoinVM::execute(std::vector<unsigned char> op_codes)
{
    uint64_t  pc=0;
    while(pc<op_codes.size())
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
                v.str = "hello"; //TODO: generate
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

                auto v1 = stack_.top();
                stack_.pop();

                auto v2 = stack_.top();
                stack_.pop();

                auto valid = valid_signature(v1.data, v2.data, v0.data);
                variable v;
                v.type_ = value_type::BOOLEAN;
                v.value_.bval = valid;
                stack_.push(v);
                pc++;
            }
            break;
            default:
                if (op_codes[pc] >=1 && op_codes[pc] <= 75)
                {
                    uint8_t data_len = op_codes[pc];
                    variable v;
                    v.type_ = value_type::DATA;
                    v.data = std::vector<unsigned char>(&op_codes[pc+1], &op_codes[pc+1+data_len]);
                    stack_.push(v);
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