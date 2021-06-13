//
// Created on 6/12/21.
//
#include <Block.h>
#include "catch2.hpp"

TEST_CASE("Test Block unpack/pack", "[Block]")
{
    std::string block_str = "01000000f06d21fc226cecc012f709ada08c618c8e27d04ff80c4d295980fd9000000000ea43df68eddf1298"
                              "9cbe851ec3b1b516fab88dc7eacfa8c81ebb59786826572ac1be7049ffff001d1f17fc010101000000010000"
                              "000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d015bffff"
                              "ffff0100f2052a0100000043410415ca91c387efac4ea86f0196b2a1d831f75a488f115055636f0022c51df5"
                              "08197ad4fc31f553d48052b05b0a3b6030a84a0441adae97734129bb8ea0ddfd4004ac00000000";
    Block block;
    std::stringstream ss(block_str);
    block.unpack(ss);
    std::stringstream ss1;
    block.pack(ss1);
    std::string packed_str = ss1.str();
    REQUIRE(block_str.compare(packed_str) == 0);
}
