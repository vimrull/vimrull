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

#include "ProtocolElement.h"
#include "BlockHeader.h"
#include "Transaction.h"

class Block: ProtocolElement
{
public:
	BlockHeader header;
	int64_t transaction_count;
	std::vector<Transaction> transactions;

	Block();
	virtual ~Block();

	virtual void unpack_hex(std::stringstream &ss);
	virtual int pack_hex(std::stringstream &ss);
	virtual bool is_valid();
	virtual unsigned char * CalculateMerkleRoot();
};
