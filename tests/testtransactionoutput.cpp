#include "catch2.hpp"
#include <TransactionOutput.h>

TEST_CASE("Test transaction output unpack/pack", "[TransactionOutput]")
{
    std::string txinput_str = "0000000000000000266a24b9e11b6d44d484862b383f5438f43ff669954d4e7133fca6a663cc23bdbbb2dfa07"
                              "457b6";

    TransactionOutput txo;
    std::stringstream ss(txinput_str);
    txo.unpack(ss);
    std::stringstream ss1;
    txo.pack(ss1);
    std::string packed_str = ss1.str();
    REQUIRE(txinput_str.compare(packed_str) == 0);
}
