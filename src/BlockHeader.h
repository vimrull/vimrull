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

class BlockHeader
{
public:
	int32_t version;
	unsigned char prev_block[32];
	unsigned char merkle_root[32];
	uint32_t timestamp;
	uint32_t bits;
	uint32_t nonce;

	BlockHeader();
	~BlockHeader();
	virtual void unpack(std::stringstream &ss);
	int pack(std::stringstream &ss);

    virtual bool is_valid();
};