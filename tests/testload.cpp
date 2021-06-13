#include <iostream>
#include <sstream>
#include <bitset>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <vector>
#include <string>
#include <stdlib.h>
#include <memory>
#include <cstdint>
#include <cstring>

#include "load.h"
#include "BlockHeader.h"
#include "Transaction.h"
#include "sha256.h"

#include "Block.h"

void test_pack_var_int()
{
    unsigned char output[10];
    int len = 10;
    int64_t val = 0;
    pack_var_int(output, len, val);

    assert(len == 9);
    assert(output[0] == 0);
    std::stringstream ss;
    hexdump(ss, output, 10 - len);
    std::cout << ss.str() << std::endl;
    //assert(strcmp((const char*)"00", ) == 0);

    auto rvalue = read_var_int(ss);
    assert(rvalue == val);

    len = 10;
    val = 1;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    assert(len == 9);
    assert(output[0] == val);
    hexdump(ss, output, 10 - len);
    std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    assert(rvalue == val);

    len = 10;
    val = 0xFC;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    assert(len == 9);
    assert(output[0] == val);
    hexdump(ss, output, 10 - len);
    std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    assert(rvalue == val);

    len = 10;
    val = 0xFD;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    assert(len == 7);
    assert(output[0] == val && output[1] == val && output[2] == 0);
    hexdump(ss, output, 10 - len);
    std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    assert(rvalue == val);

    len = 10;
    val = 0xFFFFFFFF;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    assert(len == 5);
    assert(output[0] == 0xFE && output[1] == 0xFF && output[2] == 0xFF && output[3] == 0xFF && output[4] == 0xFF);
    hexdump(ss, output, 10 - len);
    std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    assert(rvalue == val);

    len = 10;
    val = 0x100000000;
    pack_var_int(output, len, val);

    ss.str(""); ss.clear();
    assert(len == 1);
    assert(output[0] == 0xFF && output[1] == 0 && output[2] == 0 && output[3] == 0 && output[5] == 1);
    hexdump(ss, output, 10 - len);
    std::cout << ss.str() << std::endl;
    rvalue = read_var_int(ss);
    assert(rvalue == val);
}

void test()
{
    /*
	BlockHeader header;
	std::stringstream ss;
	std::string filename("block5.hex");

	read_block_file(filename, ss);

	header.unpack(ss);
	std::stringstream header_stream;
	header.pack(header_stream);
	print_hash((const char *)header_stream.str().c_str(), 80);

	int64_t tx_count = read_var_int(ss);
	std::cout << tx_count << std::endl;

	std::stringstream
	header.pack(pos);
	pos += 80;
	//hexdump((unsigned char *)data, 80);

	pos += pack_var_int((unsigned char *)pos, len, tx_count);

	Transaction tx;
	tx.unpack(ss);
	//print_coinbase_tx(tx);
	tx.print();

	tx.pack(pos, len);

	hexdump((unsigned char *)data, 1000 - len);

	char txd[1500];
	len = 1500;
	tx.pack(txd, len);


	print_hash(txd, 1500 - len);

	Transaction tx1;
	tx1.unpack(ss);
	//read_tx(ss, tx1);
	//print_tx(tx1);

	len = 1500;
	tx1.pack(txd, len);
	//hexdump((unsigned char *)txd, 1500 - len);

	print_hash(txd, 1500 - len); // Tx after Txc of block3 is 6c8a94136d2ec49ab4372d983f144bb9fb02a5a1fb6c08e4e4ef56ef4ab339b4

	// Block: 000000000000000003e6d3647001d1f455d585cdd3c298093902fe52ea4529ba
	// TxC:   82b5cf748d349d99efc36699319218142ce32b55f1fb539d192b9a3b7a2f0237
	// TxC:   8121713d86967c4b3e901a0d43c5b064443a77349cd9830e38cc10df8cb50b2d
	// Tx:    6c8a94136d2ec49ab4372d983f144bb9fb02a5a1fb6c08e4e4ef56ef4ab339b4
    */
}


#include "sha256_btc.h"

void test_sha256() {
    CSHA256 sha;
    auto test = "1234556jfhgkjdhgdf";

    sha.Write((const unsigned char *)test, strlen(test));
    unsigned char hash[32], hash2[32];
    sha.Finalize(hash);
    sha.Reset();
    sha.Write(hash, 32);
    sha.Finalize(hash2);
    print_hash(test, strlen(test));
    hexdump(hash2, 32);
}
