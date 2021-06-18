#include <tests/catch2.hpp>

#include <string>
#include <algorithm>
#include <cassert>
#include <stdio.h>
#include <stack>
#include <filesystem>

#include <picojson/picojson.h>
#include <hashlib/sha256_btc.h>
#include <hashlib/ripemd160.h>

#include "load.h"
#include "Block.h"

TEST_CASE("Test ripemd160 hash", "[ripemd160]")
{
    std::string data="The quick brown fox jumps over the lazy dog";
    int len = data.length();
    char *hash = (char *)malloc(21);
    CRIPEMD160 ripemd160;
    ripemd160.Write((const unsigned char *)data.c_str(), len);
    ripemd160.Finalize((unsigned char*) hash);
    std::string msg = hexdump(hash, 20);
    REQUIRE(msg.compare("37f332f68db77bd9d7edd4969571ad671cf9dd3b") == 0);
}

