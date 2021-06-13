#include <iostream>
#include <sstream>
#include <bitset>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string>
#include <stdlib.h>

#include "sha256.h"
#include "load.h"
#include "BlockHeader.h"
#include "Transaction.h"

using namespace std;

int hex2bin(unsigned char *dest, unsigned char *src, int len);

int hexdump(stringstream &ss, unsigned char *data, int len);

void read_block_file(string filename, stringstream &ss) {
    std::ifstream ifs(filename);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    ss << content;
}

int read(stringstream &ss, int size, void *dst) {
    assert(size > 0 && size <= 10000);
    void *data = malloc(size * 2);
    ss.read((char *) data, size * 2);

    hex2bin((unsigned char *) dst, (unsigned char *) data, size * 2);
    free(data);
    return 0;
}

int pack_hex(stringstream &ss, void *src, size_t size) {
    assert(size > 0 && size <= 10000);
    return hexdump(ss, (unsigned char *) src, size);
}

int32_t read_int32(stringstream &ss) {
    int32_t ret = 0;
    read(ss, 4, &ret);
    return ret;
}

int64_t readInt64(stringstream &ss) {
    int64_t ret = 0;
    read(ss, 8, &ret);
    return ret;
}

int64_t read_var_int(stringstream &ss) {
    char data[16];
    ss.read(data, 2);

    int64_t ret = 0;

    hex2bin((unsigned char *) &ret, (unsigned char *) data, 2);

    if (ret < 0xFD) {
        return ret;
    } else if (ret == 0xFD) {
        ss.read(data, 2 * 2);
        hex2bin((unsigned char *) &ret, (unsigned char *) data, 2 * 2);
    } else if (ret == 0xFE) {
        ss.read(data, 4 * 2);
        hex2bin((unsigned char *) &ret, (unsigned char *) data, 4 * 2);
    } else if (ret == 0xFF) {
        ss.read(data, 8 * 2);
        hex2bin((unsigned char *) &ret, (unsigned char *) data, 8 * 2);
    }

    return ret;
}

std::pair<bool, int64_t> readWitnessAndInputCount(std::stringstream &ss) {
    char data[16];
    ss.read(data, 2);

    int64_t ret = 0;
    int16_t flags;

    hex2bin((unsigned char *) &ret, (unsigned char *) data, 2);

    if (ret < 0xFD) {
        if (ret == 0) {
            ss.read(data, 2);
            hex2bin((unsigned char *) &flags, (unsigned char *) data, 2);
            assert(flags == 1);
            ret = read_var_int(ss);
        }
    } else if (ret == 0xFD) {
        ss.read(data, 2 * 2);
        hex2bin((unsigned char *) &ret, (unsigned char *) data, 2 * 2);
    } else if (ret == 0xFE) {
        ss.read(data, 4 * 2);
        hex2bin((unsigned char *) &ret, (unsigned char *) data, 4 * 2);
    } else if (ret == 0xFF) {
        ss.read(data, 8 * 2);
        hex2bin((unsigned char *) &ret, (unsigned char *) data, 8 * 2);
    }

    return std::make_pair(flags == 1, ret);
}

string readSlice(stringstream &ss, int sliceSize) {
    std::string slice(sliceSize, ' ');
    ss.read(&slice[0], sliceSize);
    return slice;
}

string readVarSlice(stringstream &ss) {
    auto sliceSize = read_var_int(ss);
    //cout << "Slice size " << sliceSize << endl;
    std::string slice(sliceSize, ' ');
    ss.read(&slice[0], sliceSize);
    return slice;
}

int pack_var_int(unsigned char *output, int &output_len, int64_t val) {
    int size = 0;
    char byte1;

    if (val < 0xFD) {
        size = 1;
        char tval = (val & 0xFF);
        std::memcpy(output, &tval, 1);
    } else if (val <= 0xFFFF) {
        size = 3;
        int16_t tval = (val & 0xFFFF);
        byte1 = 0xFD;
        std::memcpy(output, &byte1, 1);
        std::memcpy(output + 1, &tval, 2);
    } else if (val <= 0xFFFFFFFF) {
        size = 5;
        int32_t tval = (val & 0xFFFFFFFF);
        byte1 = 0xFE;
        std::memcpy(output, &byte1, 1);
        std::memcpy(output + 1, &tval, 4);
    } else {
        size = 9;
        byte1 = 0xFF;
        std::memcpy(output, &byte1, 1);
        std::memcpy(output + 1, &val, 8);
    }

    output_len -= size;
    return size;
}

int pack_var_int(std::stringstream &ss_target, int64_t val) {
    int size = 0;
    char byte1;
    std::stringstream ss;

    if (val < 0xFD) {
        size = 1;
        char tval = (val & 0xFF);
        ss.write((const char *) &tval, 1);
        // std::memcpy(output, &tval, 1);
    } else if (val <= 0xFFFF) {
        size = 3;
        int16_t tval = (val & 0xFFFF);
        byte1 = 0xFD;
        ss.write((const char *) &byte1, 1);
        //std::memcpy(output, &byte1, 1);
        ss.write((const char *) &tval, 2);
        //std::memcpy(output+1, &tval, 2);
    } else if (val <= 0xFFFFFFFF) {
        size = 5;
        int32_t tval = (val & 0xFFFFFFFF);
        byte1 = 0xFE;
        ss.write((const char *) &byte1, 1);
        //std::memcpy(output, &byte1, 1);
        ss.write((const char *) &tval, 2);
        //std::memcpy(output + 1, &tval, 4);
    } else {
        size = 9;
        byte1 = 0xFF;
        ss.write((const char *) &byte1, 1);
        //std::memcpy(output, &byte1, 1);
        ss.write((const char *) &val, 8);
        //std::memcpy(output + 1, &val, 8);
    }
    std::string ss_str = ss.str();
    return hexdump(ss_target, (unsigned char *) ss_str.c_str(), ss_str.length());
}

// we need a helper function to convert hex to binary, this function is unsafe and slow,
// but very readable (pack_hex something better)
int hex2bin(unsigned char *dest, unsigned char *src, int len) {
    assert(len > 0);

    unsigned char bin;
    int c, pos;
    char buf[3];

    pos = 0;
    c = 0;
    buf[2] = 0;

    while (c < len) {
        // read in 2 characaters at a time
        buf[0] = src[c++];
        buf[1] = src[c++];
        // convert them to a interger and recast to a char (uint8)
        dest[pos++] = (unsigned char) strtol(buf, NULL, 16);
    }

    return pos;
}

void hexdump(unsigned char *data, int len) {
    int c = 0;
    while (c < len) {
        cout << hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[c++]);
    }
    printf("\n");
}

int hexdump(stringstream &ss, unsigned char *data, int len) {
    int c = 0;
    while (c < len) {
        ss << hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[c++]);
    }
    return len * 2;
}

void byte_swap(unsigned char *data, int len) {
    // O(N) - I can not think of better way
    for (int i = 0; i < len - 1; i += 2) {
        char temp = data[i];
        data[i] = data[i + 1];
        data[i + 1] = temp;
    }
}


unsigned char * dhash(unsigned char *data, long len) {
    char *hash = (char *)malloc(64), *hash1 = (char* )malloc(64);
    sha256((const char *)data, &hash, &len);
    sha256((const char *)hash, &hash1, &len);

    free(hash);
    return (unsigned char *) hash1;
}

void print_hash(const char *data, long len) {
    unsigned char *hash = dhash((unsigned char *)data, len);

    std::reverse((unsigned char *)hash, (unsigned char *)hash + 32);
    hexdump((unsigned char *)hash, 32);
    free(hash);
}

unsigned char * calculate_merkle_root(std::vector<unsigned char *> *txhashes)
{
    int count = txhashes->size();
    if (count == 1)
    {
        return txhashes->at(0);
    }
    std::vector<unsigned char *> *txnew = new std::vector<unsigned char *>();

    if (count % 2)
    {
        txhashes->push_back(txhashes->at(count - 1));
        count++;
    }

    for (int i = 0;i < count;i += 2)
    {
        unsigned char * d1 = txhashes->at(i);
        unsigned char * d2 = txhashes->at(i+1);

        unsigned char concat[64];
        std::memcpy(concat, d1, 32);
        std::memcpy(&concat[32], d2, 32);
        auto hash = dhash(concat, 64);
        txnew->push_back(hash);

        //free(d1);
        //free(d2);
    }
    return calculate_merkle_root(txnew);
}

void test_calculate_merkle_root()
{
    /*
	BlockHeader header;
	std::stringstream ss;
	std::string filename("data/input/block1.hex");

	read_block_file(filename, ss);

	header.unpack(ss);
	char data[1000];
	int len = 1000;
	header.pack(data, len);
	print_hash((const char *)data, 80);

	int64_t tx_count = read_var_int(ss);
	std::cout << tx_count << std::endl;

	char *pos = data;
	len = 1000;
	header.pack(pos, len);
	pos += 80;
	//hexdump((unsigned char *)data, 80);

	pos += pack_var_int((unsigned char *)pos, len, tx_count);
	const int datasize = 1024 * 1024 * 8;
	unsigned char *txd  = (unsigned char *) malloc(datasize);

	std::vector<unsigned char *> transactions;

	std::cout << "------------- List ----------\n";

	for (int i = 0;i < tx_count;i++)
	{
		Transaction tx;
		tx.unpack(ss);

		len = datasize;
		tx.pack((char *)txd, len);
		auto hash = dhash(txd, datasize - len);
		transactions.push_back(hash);
		//hexdump(hash, 32);
	}

	std::cout << std::dec<< transactions.size() << std::endl;
	/*
	char m[64];
	memcpy(m, transactions[0], 32);

	memcpy(&m[32], transactions[1], 32);

	auto hash = dhash((unsigned char *)m, 64);
	hexdump(hash, 32);

	memcpy(m, transactions[2], 32);
	memcpy(&m[32], transactions[2], 32);

	auto hash2 = dhash((unsigned char *)m, 64);
	hexdump(hash2, 32);

	memcpy(m, hash, 32);
	memcpy(&m[32], hash2, 32);

	auto hash3 = dhash((unsigned char *)m, 64);
	hexdump(hash3, 32);
	* /

	std::cout << "------------- Merkle Root ----------\n";
	auto mr = calculate_merkle_root(&transactions);
	hexdump(mr, 32);
    */
}

void test_calculate_merkle_root2()
{
    /*
	Block block;
	std::stringstream ss;
	std::string filename("data/input/block1.hex");

	read_block_file(filename, ss);
	std::vector<unsigned char *> transactions;
	block.unpack(ss);

	auto mr = block.CalculateMerkleRoot();
	hexdump(mr, 32);
    */
}

int mainxx(int argc, char *argv[])
{
    // test_pack_var_int();
    // test();
    test_calculate_merkle_root2();
    return 0;
}

//https://en.bitcoin.it/wiki/Difficulty
#include <iostream>
#include <cmath>

inline float fast_log(float val)
{
    int * const exp_ptr = reinterpret_cast <int *>(&val);
    int x = *exp_ptr;
    const int log_2 = ((x >> 23) & 255) - 128;
    x &= ~(255 << 23);
    x += 127 << 23;
    *exp_ptr = x;

    val = ((-1.0f / 3) * val + 2) * val - 2.0f / 3;
    return ((val + log_2) * 0.69314718f);
}

float difficulty(unsigned int bits)
{
    static double max_body = fast_log(0x00ffff), scaland = fast_log(256);
    return exp(max_body - fast_log(bits & 0x00ffffff) + scaland * (0x1d - ((bits & 0xff000000) >> 24)));
}