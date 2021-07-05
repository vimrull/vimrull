#include <iostream>
#include <filesystem>
#include "catch2/catch2.hpp"
#include "db/database.h"
#include "load.h"
#include "Block.h"

TEST_CASE("Test create sqlite3 database and create table, insert and load a row", "sqlite3")
{
    std::string tx_db_filename = "bitcoindata/db/test01.db";
    database db(tx_db_filename);
    REQUIRE(db.create_tx_table());
    std::string tx_id="txid1";
    std::string block_id="block1";
    int block_height = 10;
    REQUIRE(db.insert_tx_table(tx_id, block_id, block_height));

    std::string tx_id_loaded, block_id_loaded;
    auto tx = db.load_tx(tx_id);

    REQUIRE((tx_id == std::any_cast<std::string>(tx["id"])
            && block_id == std::any_cast<std::string>(tx["block_id"])
            && block_height == std::any_cast<int>(tx["block_height"])
            ));

    std::filesystem::remove(std::filesystem::path(tx_db_filename));
}

TEST_CASE("Test block tx insertion", "[database]")
{
    const std::string tx_db_filename = "bitcoindata/db/test01.db";
    database db(tx_db_filename);
    REQUIRE(db.create_tx_table());

    //TODO: find a way to access local variables in this function
    const finalizer fin([] (std::any param) {
        std::string fn = std::any_cast<std::string>(param);
        std::filesystem::remove(std::filesystem::path(fn));
    }, tx_db_filename);

    std::string prev_block = "00000000edfa5bfffd21cc8ce76e46b79dc00196e61cdc62fd595316136f8a83"; // block 1024
    int tx_count = 0;
    bool found_genesis = false;
    int block_height = 1024;
    while (!found_genesis)
    {
        std::string file_name = "bitcoindata/rawblock/" + prev_block + ".hex";
        REQUIRE(std::filesystem::exists(file_name));  // run downloader first (testinit)
        std::stringstream ss;
        read_block_file(file_name, ss);
        Block block;
        block.unpack_hex(ss);

        REQUIRE(block.is_valid());
        for(auto tx: block.transactions) {
            for(auto txo: tx.outputs) {
                auto tx_id = tx.calculate_id();
                std::string tx_id_str = hexdump((void *)(tx_id.data()), tx_id.size());
                //std::cout << tx_id_str << std::endl;
                db.insert_tx_table(tx_id_str, prev_block, block_height);
                tx_count++;
            }
        }

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

        if (std::string(prev_block).compare(BITCOIN_GENESIS_BLOCK_HASH) != 0)
        {
            // move next
            std::reverse((unsigned char *) block.header.prev_block, (unsigned char *) block.header.prev_block + 32);
            std::stringstream ssf;
            hexdump(ssf, block.header.prev_block, 32);
            file_name = ssf.str();
            prev_block = file_name;
        }
        else
        {
            found_genesis = true;
        }
        block_height--;
    }
    //std::cout << "Total transactions found: " << tx_count << std::endl;
    REQUIRE(tx_count == 1054);
    REQUIRE(found_genesis);
}
