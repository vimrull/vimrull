#include <tests/catch2.hpp>
#include "load.h"
#include "crypto/util.h"

TEST_CASE("Test validate known signature", "[openssl]")
{
    std::string tx_hex  = "bd2affabe389cd4182b3a0888df12261999f0afb7894fc8ef44947d277fbe781";
    std::string sig_hex = "3045022044ef433a24c6010a90af14f7739e7c60ce2c5bc3eab96eaee9fbccfd"
                          "bb3e2722022100ac8d59e834dca2f5f13d7762403520e8d9a64c96a23053ade5"
                          "41e53e023eacaf";
    std::string pk_hex = "03a2fef1829e0742b89c218c51898d9e7cb9d51201ba2bf9d9e9214ebb6af32708";

    std::vector<unsigned char> pubkey(pk_hex.length()/2);
    std::vector<unsigned char> hash(sig_hex.length()/2);
    hex2bin(pubkey.data(), (unsigned char *) pk_hex.data(), pk_hex.length());
    std::vector<unsigned char> sig(sig_hex.length()/2);
    hex2bin(sig.data(), (unsigned char *) sig_hex.data(), sig_hex.length());
    hex2bin(hash.data(), (unsigned char *) tx_hex.data(), tx_hex.length());

    REQUIRE(valid_signature(hash, sig, pubkey));

}

