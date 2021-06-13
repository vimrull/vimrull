#include <BlockHeader.h>
#include "catch2.hpp"

TEST_CASE("Test block header unpack/pack", "[BlockHeader]")
{
    std::string header_str = "0400e020d9ce1aeec4c3f7203da87792de66003f4981bb2ece110a000000000000000000ab7ac4fb36da8bb9"
                             "a0a8d7c52bcb4c6a3f815e61fd552d9d1e3d2549306393d4c62ac4607b5f0d17682a87ce";

    BlockHeader header;
    std::stringstream ss(header_str);
    header.unpack(ss);
    std::stringstream ss1;
    header.pack(ss1);
    std::string packed_str = ss1.str();
    REQUIRE(header_str.compare(packed_str) == 0);
}
