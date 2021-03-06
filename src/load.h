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
#include <any>
//#define VALIDATION_FAIL_MSG std::cout << "Validation failed at: " << __FILE__ << ":" << __LINE__ << std::endl;
#define VALIDATION_FAIL_MSG

#define ENSURE_IS_VALID(x) if(!(x)) { VALIDATION_FAIL_MSG; return false; }

class TransactionInput;
class TransactionOutput;
class Transaction;

typedef uint64_t varint;

void byte_swap(unsigned char *data, int len);
void hexdump(unsigned char *data, int len);
std::string hexdump(void *data, int len);
int pack_var_int(unsigned char *output, int &output_len, int64_t val);
int hex2bin(unsigned char *dest, unsigned char *src, int len);
int hexdump(std::stringstream &ss, unsigned char *data, int len);
int64_t read_var_int(std::stringstream &ss);
std::pair<bool, int64_t> readWitnessAndInputCount(std::stringstream &ss);
bool read_block_file(std::string filename, std::stringstream &ss);
int pack_var_int(unsigned char *output, int &output_len, int64_t val);
int pack_var_int(std::stringstream &ss, int64_t val);
int pack_var_int(std::vector<char> &output, int64_t val);
int read(std::stringstream &ss, int size, void* dst);
int pack_ptr2hex(std::stringstream &ss, void *src, size_t size);
int32_t read_int32(std::stringstream &ss);
int64_t readInt64(std::stringstream &ss);
int64_t read_var_int(std::stringstream &ss);
int hex2bin(unsigned char *dest, unsigned char *src, int len);
void print_hash(const char *data, long len);
unsigned char * dhash(unsigned char *data, long len);

#define SIGHASH_ALL 0x00000001

class finalizer
{
public:
    finalizer(void (*pFinal)(std::any), std::any param) {
        this->pFinal = pFinal;
        this->param = param;
    }

    ~finalizer()
    {
        if(pFinal)
        {
            pFinal(param);
        }
    }

private:
    std::any param;
    void (*pFinal)(std::any);
};