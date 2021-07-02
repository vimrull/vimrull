#include "../include/catch2/catch2.hpp"
#include <assert.h>
#include "Witness.h"

TEST_CASE("Test witness unpack_hex/pack_hex", "[Witness]")
{
    Witness w;
    std::string witness_str = "01200000000000000000000000000000000000000000000000000000000000000000";
    std::stringstream ss(witness_str);
    auto wlist = w.unpack_list(ss);
    std::stringstream ss1;
    w.pack_list(ss1, wlist);
    std::string packed_str = ss1.str();
    REQUIRE(witness_str.compare(packed_str) == 0);
}