#include <iostream>
#include <filesystem>
#include <sqlite3.h>
#include "catch2/catch2.hpp"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++)
    {
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    //printf("\n");
    return 0;
}

TEST_CASE("Test create sqlite3 database and create a table", "sqlite3")
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    std::string tx_db_filename = "bitcoindata/db/tx.db";

    rc = sqlite3_open(tx_db_filename.c_str(), &db);
    if (rc)
    {
        FAIL();
        sqlite3_close(db);
        return;
    }
    std::string sql = "CREATE TABLE IF NOT EXISTS TX(id TEXT PRIMARY KEY NOT NULL)";
    rc = sqlite3_exec(db,  sql.c_str(), callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        FAIL();
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);
    std::filesystem::remove(std::filesystem::path(tx_db_filename));
}