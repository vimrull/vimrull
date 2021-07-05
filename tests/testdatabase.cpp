#include <iostream>
#include <filesystem>
#include "catch2/catch2.hpp"
#include "db/database.h"


TEST_CASE("Test create sqlite3 database and create a table", "sqlite3")
{
    std::string tx_db_filename = "bitcoindata/db/tx.db";
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
