
#include <tests/catch2.hpp>
#include <vm/opcodes.h>
#include <vm/variable.h>
#include <vm/BitcoinVM.h>
#include "load.h"

TEST_CASE("Test standard script", "[vm]")
{
    // TODO: put actual address and signature
    std::string op_codes_str="76A914AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA88AC";
    std::vector<unsigned char> op_codes;
    op_codes.resize(op_codes_str.length() / 2 + 1);
    hex2bin((unsigned char *)op_codes.data(), (unsigned char *)op_codes_str.c_str(), op_codes_str.length());
    auto vm = std::make_unique<BitcoinVM>();
    variable v_sig, v;
    v_sig.type_ = value_type::DATA;
    v_sig.data = {'a', 'b', 'c'};
    vm->stack_.push(v_sig);
    v.type_ = value_type::STRING;
    v.str = "hello";
    vm->stack_.push(v);
    REQUIRE(vm->execute(op_codes));
}