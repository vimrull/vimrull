#ifndef BITCOIN_DATABASE_H
#define BITCOIN_DATABASE_H

#include <iostream>
#include <map>
#include <any>
#include <sqlite3.h>

class database
{
public:
    database(std::string path);
    virtual ~database();
    virtual bool create_tx_table();

    bool insert_tx_table(std::string basicString, std::string basicString1, int i);

    std::map<std::string, std::any> load_tx(std::string tx_id);

private:
    sqlite3 *connection;
};

#endif //BITCOIN_DATABASE_H
