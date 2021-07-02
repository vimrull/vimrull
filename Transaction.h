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
#include "load.h"
#include "TransactionInput.h"
#include "TransactionOutput.h"
#include "ProtocolElement.h"

class Transaction : ProtocolElement
{
public:
	int32_t version;
	bool has_witness;
	int64_t input_count; //todo: varint
	std::vector<TransactionInput> inputs;
	int64_t output_count; //todo: varint
	std::vector<TransactionOutput> outputs;
	uint32_t locktime;

	Transaction();
	virtual ~Transaction();
	virtual int pack_hex(std::stringstream &ss);
	virtual void unpack_hex(std::stringstream &ss);
	bool allow_witness();
	virtual void print();

	virtual bool is_valid();

    bool pack_hex_for_validation(std::stringstream stringstream, std::vector<unsigned char> vector);
};
