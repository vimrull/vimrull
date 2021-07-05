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

#define BITCOIN_GENESIS_BLOCK_HASH "00000000839a8e6886ab5951d76f411475428afc90947ee320161bbf18eb6048"

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
