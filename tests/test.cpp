#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2.hpp"

#include <string>
#include <algorithm>
#include <cassert>
#include <stdio.h>
#include <stack>
#include <filesystem>

#include "load.h"
#include "Block.h"

#define GENESIS_BLOCK_HASH "00000000839a8e6886ab5951d76f411475428afc90947ee320161bbf18eb6048"

TEST_CASE("Test varint functions", "[varint functions]")
{
    unsigned char output[10];
    int len = 10;
    int64_t val = 0;
    pack_var_int(output, len, val);

    REQUIRE(len == 9);
    REQUIRE(output[0] == 0);
    std::stringstream ss;
    hexdump(ss, output, 10 - len);

    auto rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 1;
    pack_var_int(output, len, val);

    ss.str("");
    ss.clear();
    REQUIRE(len == 9);
    REQUIRE(output[0] == val);
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0xFC;
    pack_var_int(output, len, val);

    ss.str("");
    ss.clear();
    REQUIRE(len == 9);
    REQUIRE(output[0] == val);
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0xFD;
    pack_var_int(output, len, val);

    ss.str("");
    ss.clear();
    REQUIRE(len == 7);
    REQUIRE((output[0] == val && output[1] == val && output[2] == 0));
    hexdump(ss, output, 10 - len);
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0xFFFFFFFF;
    pack_var_int(output, len, val);

    ss.str("");
    ss.clear();
    REQUIRE(len == 5);
    REQUIRE((output[0] == 0xFE && output[1] == 0xFF && output[2] == 0xFF && output[3] == 0xFF && output[4] == 0xFF));
    hexdump(ss, output, 10 - len);
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0x100000000;
    pack_var_int(output, len, val);

    ss.str("");
    ss.clear();
    REQUIRE(len == 1);
    REQUIRE((output[0] == 0xFF && output[1] == 0 && output[2] == 0 && output[3] == 0 && output[5] == 1));
    hexdump(ss, output, 10 - len);
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);
}

TEST_CASE("Test block traversing", "[block loop]")
{
    std::string next_block = "00000000edfa5bfffd21cc8ce76e46b79dc00196e61cdc62fd595316136f8a83"; // block 1024

    bool found_genesis = false;
    while (!found_genesis) {
        std::string file_name = "bitcoindata/rawblock/"+next_block+".hex";
        REQUIRE(std::filesystem::exists(file_name));  // run downloader first (testinit)
        std::stringstream ss;
        read_block_file(file_name, ss);
        Block block;
        block.unpack(ss);

        std::stringstream output, out, outheader;
        block.pack(output);
        auto outstr = output.str();
        hexdump(out, (unsigned char *) outstr.c_str(), outstr.length());

        std::stringstream headerss;
        block.header.pack(headerss);
        std::string headerstr = headerss.str();
        hexdump(outheader, (unsigned char *) headerstr.c_str(), headerstr.length());
        long lenh = outheader.str().length();
        assert(lenh == 320);
        char outh[160];
        hex2bin((unsigned char *) outh, (unsigned char *) outheader.str().c_str(), lenh);

        if (std::string(next_block).compare(GENESIS_BLOCK_HASH) != 0)
        {
            // move next
            std::reverse((unsigned char *) block.header.prev_block, (unsigned char *) block.header.prev_block + 32);
            std::stringstream ssf;
            hexdump(ssf, block.header.prev_block, 32);
            file_name = ssf.str();
            next_block = file_name;
            //std::cout << next_block << std::endl;
        }
        else
        {
            found_genesis = true;
        }
    }

    REQUIRE(found_genesis);
}
