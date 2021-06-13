#pragma once
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

#include "BlockHeader.h"

class CoinbaseTransactionInput;
class CoinbaseTransaction;
class TransactionInput;
class TransactionOutput;
class Transaction;

typedef uint64_t varint;

void byte_swap(unsigned char *data, int len);
void hexdump(unsigned char *data, int len);
int pack_var_int(unsigned char *output, int &output_len, int64_t val);
int hex2bin(unsigned char *dest, unsigned char *src, int len);
int hexdump(std::stringstream &ss, unsigned char *data, int len);
int64_t read_var_int(std::stringstream &ss);
std::pair<bool, int64_t> readWitnessAndInputCount(std::stringstream &ss);
void read_block_file(std::string filename, std::stringstream &ss);
int pack_var_int(unsigned char *output, int &output_len, int64_t val);
int pack_var_int(std::stringstream &ss, int64_t val);
int read(std::stringstream &ss, int size, void* dst);
int pack_hex(std::stringstream &ss, void *src, size_t size);
int32_t read_int32(std::stringstream &ss);
int64_t readInt64(std::stringstream &ss);
int64_t read_var_int(std::stringstream &ss);
int hex2bin(unsigned char *dest, unsigned char *src, int len);
void print_hash(const char *data, long len);