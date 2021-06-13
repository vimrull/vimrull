//
// Created on 6/12/21.
//
#include <Outpoint.h>
#include "catch2.hpp"

TEST_CASE("Test Outpoint unpack_hex/pack_hex", "[Outpoint]")
{
    std::string outpoint_str = "05ed24245b473b420089df5e8c7dccdc95b154ed7a5f03e6ba108f4c2654400840000000";
    Outpoint outpoint;
    std::stringstream ss(outpoint_str);
    outpoint.unpack_hex(ss);
    std::stringstream ss1;
    outpoint.pack_hex(ss1);
    std::string packed_str = ss1.str();
    REQUIRE(outpoint_str.compare(packed_str) == 0);
}
