#pragma once
#include <iostream>
#include <sstream>
#include <bitset>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <vector>
#include <string>
#include <stdlib.h>
#include <memory>

#include "ProtocolElement.h"
#include "Outpoint.h"
#include "Witness.h"

class TransactionInput : ProtocolElement
{
public:
	Outpoint previous_output;
	uint32_t script_size;
	unsigned char *script;
	uint32_t sequence;
	int64_t witness_count;
    std::vector<Witness> witness_list;

	// Only if the input is coinbase input
	int block_height;

	virtual void unpack_hex(std::stringstream &ss);
	virtual int pack(std::vector<char> output);
    virtual int pack_hex(std::stringstream &ss);
	virtual void ReadBlockHeight();

	TransactionInput();
	virtual ~TransactionInput();

	virtual bool IsCoinbase();
    virtual bool is_valid();
};