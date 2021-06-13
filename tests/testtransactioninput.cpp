#include "catch2.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <stdio.h>
#include <stack>

#include "load.h"
#include "BlockHeader.h"
#include "Transaction.h"
#include "sha256.h"
#include "Block.h"

TEST_CASE("Test transaction input unpack/pack", "[TransactionInput]")
{
    std::string txinput_str = "0000000000000000000000000000000000000000000000000000000000000000ffffffff4f03887c0a04587f"
                              "4126646766737a2f4254432e636f6d2ffabe6d6db973e3b8c44779b885492064972fc115ffc533ab818a0f3c7"
                              "88470158a31645302000000020bb56c0800b09a1210000000000000ffffffff";
    TransactionInput txi;
    std::stringstream ss(txinput_str);
    txi.unpack(ss);
    std::stringstream ss1;
    txi.pack(ss1);
    std::string packed_str = ss1.str();
    REQUIRE(txinput_str.compare(packed_str) == 0);
}
