#include <iostream>
#include "database.h"

database::database(std::string path)
{
    connection= nullptr;
    auto rc = sqlite3_open(path.c_str(), &connection);
    if (rc)
    {
        sqlite3_close(connection);
        throw new std::exception();
    }
}

database::~database()
{
    if(connection)
    {
        sqlite3_close(connection);
    }
}

static int callback(void *db_ref, int argc, char **argv, char **azColName){
    database *This = (database *) db_ref;

    int i;
    for(i=0; i<argc; i++)
    {
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    //printf("\n");
    return 0;
}


bool database::create_tx_table()
{
    std::string sql = "CREATE TABLE IF NOT EXISTS TX("
                          "id TEXT PRIMARY KEY NOT NULL,"
                          "block_id TEXT NOT NULL,"
                          "block_height INT,"
                          "UNIQUE(id, block_id)"
                      ")";
    char *zErrMsg = 0;
    auto rc = sqlite3_exec(connection, sql.c_str(), callback, (void *) this, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(zErrMsg);
        return false;
    }

    return true;
}

bool database::insert_tx_table(std::string tx_id, std::string block_id, int block_height)
{
    std::string sql = "INSERT INTO TX(id, block_id, block_height) VALUES(?,?,?)";
    sqlite3_stmt *stmt= nullptr;
    auto  result = sqlite3_prepare_v2(connection, sql.c_str(), sql.length(), &stmt,nullptr);
    if (SQLITE_OK != result) { throw std::exception(); }

    result = sqlite3_bind_text(stmt, 1, tx_id.c_str(), tx_id.length(), SQLITE_STATIC);
    if (SQLITE_OK != result) { throw std::exception(); }

    result = sqlite3_bind_text(stmt, 2, block_id.c_str(), block_id.length(), SQLITE_STATIC);
    if (SQLITE_OK != result) { throw std::exception(); }

    result = sqlite3_bind_int(stmt, 3, block_height);
    if (SQLITE_OK != result) { throw std::exception(); }
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return true;
}

std::map<std::string, std::any> database::load_tx(std::string tx_id)
{
    std::string sql = "SELECT id, block_id, block_height from TX WHERE id=?";
    sqlite3_stmt *stmt= nullptr;
    auto result = sqlite3_prepare_v2(connection, sql.c_str(), sql.length(), &stmt,nullptr);

    if (SQLITE_OK != result)
    {
        throw std::exception();
    }

    result = sqlite3_bind_text(stmt, 1, tx_id.c_str(), tx_id.length(), SQLITE_STATIC);
    if (SQLITE_OK != result) { throw std::exception(); }
    std::string block_id;
    std::map<std::string, std::any> row;
    row["id"] = tx_id;

    if((result = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        row["block_id"] = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));;
        row["block_height"] = sqlite3_column_int(stmt, 2);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return row;
}
