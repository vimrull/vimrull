#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2.hpp"

#include <string>
#include <algorithm>
#include <cassert>
#include <stdio.h>
#include <stack>
#include <filesystem>

#include <picojson/picojson.h>

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
        block.unpack_hex(ss);

        REQUIRE(block.is_valid());

        std::stringstream output, out, outheader;
        block.pack_hex(output);
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
        }
        else
        {
            found_genesis = true;
        }
    }

    REQUIRE(found_genesis);
}

TEST_CASE("Test full block with transactions", "[general]")
{
    std::string json_filename = "bitcoindata/jsonblock/0000000000000000000d06cb8554f862f69825a7994dab6161ec0970e35f463e.json";
    std::stringstream json_block_stream;
    REQUIRE(read_block_file(json_filename, json_block_stream));

    std::string hex_filename = "bitcoindata/rawblock/0000000000000000000d06cb8554f862f69825a7994dab6161ec0970e35f463e.hex";
    std::stringstream block_stream_hex;
    REQUIRE(read_block_file(hex_filename, block_stream_hex));

    Block block;
    char temp[1];
    //block_stream_hex.read(temp, 1);
    block.unpack_hex(block_stream_hex);

    REQUIRE(block.is_valid());

    picojson::value json_block;
    json_block_stream >> json_block;
    std::string err = picojson::get_last_error();

    if (! err.empty())
    {
        FAIL("FAILED to load json_block: " + err);
    }

    const picojson::value::object& obj = json_block.get<picojson::object>();

    //std::cout << obj.at("prev_block") << std::endl;
    std::reverse((unsigned char *)block.header.prev_block, (unsigned char *)block.header.prev_block + 32);
    std::string prev_block = hexdump((void *)block.header.prev_block, 32);

    std::reverse((unsigned char *)block.header.merkle_root, (unsigned char *)block.header.merkle_root + 32);
    std::string mrkl_root = hexdump((void *)block.header.merkle_root, 32);

    std::string prev_block_json = obj.at("prev_block").to_str();
    std::string mrkl_root_json = obj.at("mrkl_root").to_str();

    auto bits = (uint32_t) obj.at("bits").get<double>();
    auto version = (int32_t) obj.at("ver").get<double>();
    auto timestamp = (uint32_t) obj.at("time").get<double>();
    auto n_tx = (int64_t) obj.at("n_tx").get<double>();
    auto nonce = (uint32_t) obj.at("nonce").get<double>();

    REQUIRE(prev_block.compare(prev_block_json) == 0);
    REQUIRE(mrkl_root.compare(mrkl_root_json) == 0);
    REQUIRE(block.header.version == version);
    REQUIRE(block.header.bits == bits);
    REQUIRE(block.header.timestamp == timestamp);
    REQUIRE(block.header.nonce == nonce);
    REQUIRE(block.transaction_count == n_tx);

    auto tx_array = obj.at("tx").get<picojson::array>();
    int tx_index = 0;
    for (auto tx_it = tx_array.begin(); tx_it != tx_array.end(); ++tx_it)
    {
        auto tx = block.transactions.at(tx_index);
        auto tx_json = tx_it->get<picojson::object>();
        auto version = (int32_t) tx_json.at("ver").get<double>();
        auto vin_sz = (int64_t) tx_json.at("vin_sz").get<double>();
        auto vout_sz = (int64_t) tx_json.at("vout_sz").get<double>();
        auto lock_time = (uint32_t) tx_json.at("lock_time").get<double>();

        auto inputs = tx_json.at("inputs").get<picojson::array>();
        auto outputs = tx_json.at("out").get<picojson::array>();

        REQUIRE(tx.version == version);
        REQUIRE(tx.input_count == vin_sz);
        REQUIRE(tx.output_count == vout_sz);
        REQUIRE(tx.locktime == lock_time);

        /*
        for (auto ti = tx_json.begin(); ti != tx_json.end(); ++ti)
        {
            std::cout << ti->first << ": " << ti->second << std::endl;
        }
        */

        auto inp_idx = 0;
        for (auto inp_it = inputs.begin(); inp_it != inputs.end(); ++inp_it)
        {
            auto inp = tx.inputs.at(inp_idx);
            auto inp_json = inp_it->get<picojson::object>();
            auto sequence = (uint32_t) inp_json.at("sequence").get<double>();
            REQUIRE(inp.sequence == sequence);

            inp_idx++;
        }

        auto outp_idx = 0;
        for (auto outp_it = outputs.begin(); outp_it != outputs.end(); ++outp_it)
        {
            auto outp = tx.outputs.at(outp_idx);
            auto outp_json = outp_it->get<picojson::object>();
            auto value = (uint64_t) outp_json.at("value").get<double>();
            REQUIRE(outp.value == value);
            outp_idx++;
        }

        tx_index++;
    }
    REQUIRE(tx_index == block.transaction_count);
}
