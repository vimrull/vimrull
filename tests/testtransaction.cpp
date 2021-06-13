//
// Created on 6/12/21.
//
#include <Transaction.h>
#include "catch2.hpp"


TEST_CASE("Test transaction unpack/pack", "[TransactionOutput]")
{
    /*
    std::string tx_str = "00200000000010113a4f4e13f5ce58f5d49cd97b127a902e85ed0bf771398d1e31c41f2fed808982e00000000ffff"
                         "ffff01a65711000000000017a91464ff8d4ec34835508512f19be934aa9c273d62d98702483045022100d6c27ea89"
                         "9930a8ee9b8bedbcaa29b36a8cdecd535b44ddf20d464b8abf978420220336f36923358cdf7c2729347157bdd0a2b"
                         "26a208963aea436697d34dd40249c0012103fa87807b7bafa40f71266813ffe86315a811c1890b983ef4e8827b582"
                         "f70942000000000";
    Transaction tx;
    std::stringstream ss(tx_str);
    tx.unpack(ss);
    std::stringstream ss1;
    tx.pack(ss1);
    std::string packed_str = ss1.str();
    REQUIRE(tx_str.compare(packed_str) == 0);
    */
}
