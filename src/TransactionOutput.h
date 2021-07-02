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

class TransactionOutput : ProtocolElement
{
public:
	uint64_t value;
	int64_t script_size; //todo: varint
	unsigned char *script;

	TransactionOutput();
	virtual ~TransactionOutput();
	virtual void unpack_hex(std::stringstream &ss);
	virtual int pack(char *output, int &output_len);
    virtual int pack_hex(std::stringstream &ss);
    virtual bool is_valid();
};
