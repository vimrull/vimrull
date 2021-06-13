#include "catch2.hpp"

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <cstdint>
#include <cstring>
#include "sha256_btc.h"
#include "load.h"

TEST_CASE("Test pack_var_int", "[load]")
{
    unsigned char output[10];
    int len = 10;
    int64_t val = 0;
    pack_var_int(output, len, val);

    REQUIRE(len == 9);
    REQUIRE(output[0] == 0);
    std::stringstream ss;
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    //assert(strcmp((const char*)"00", ) == 0);

    auto rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 1;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    REQUIRE(len == 9);
    REQUIRE(output[0] == val);
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0xFC;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    REQUIRE(len == 9);
    REQUIRE(output[0] == val);
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0xFD;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    REQUIRE(len == 7);
    REQUIRE((output[0] == val && output[1] == val && output[2] == 0));
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0xFFFFFFFF;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    REQUIRE(len == 5);
    REQUIRE((output[0] == 0xFE && output[1] == 0xFF && output[2] == 0xFF && output[3] == 0xFF && output[4] == 0xFF));
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    len = 10;
    val = 0x100000000;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    REQUIRE(len == 1);
    REQUIRE((output[0] == 0xFF && output[1] == 0 && output[2] == 0 && output[3] == 0 && output[5] == 1));
    hexdump(ss, output, 10 - len);
    //std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    REQUIRE(rvalue == val);

    val = 0x100000000;
    std::vector<char> output_vec;
    len = pack_var_int(output_vec, val);
    REQUIRE(len == 9);
    //assert(output_vec[0] == 0xFF && output_vec[1] == 0 && output_vec[2] == 0 && output_vec[3] == 0 && output_vec[5] == 1);
}

TEST_CASE("Test load block file", "[load]")
{
    std::string filename = "bitcoindata/jsonblock/0000000000000000000d06cb8554f862f69825a7994dab6161ec0970e35f463e.json";
    std::stringstream block_stream;
    REQUIRE(read_block_file(filename, block_stream));
}

TEST_CASE("Test load hex block file", "[load]")
{
    std::string filename = "bitcoindata/rawblock/0000000000000000000d06cb8554f862f69825a7994dab6161ec0970e35f463e.hex";
    std::stringstream block_stream;
    REQUIRE(read_block_file(filename, block_stream));
}

TEST_CASE("Test sha256 generation", "[core]")
{
    CSHA256 sha;
    auto test = "1234556jfhgkjdhgdf";
    auto expected_hash = "7a3c8a3f7fc606e8b153ead2d69536028663b7827963c112b90c6529a966bddd";

    sha.Write((const unsigned char *)test, strlen(test));
    unsigned char hash[32];
    sha.Finalize(hash);
    sha.Reset();
    sha.Write(hash, 32);
    sha.Finalize(hash);
    std::string hash_found = hexdump((void *)hash, 32);
    REQUIRE(hash_found.compare(expected_hash) == 0);
}
