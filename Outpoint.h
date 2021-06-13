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

class Outpoint : ProtocolElement
{
public:
	char hash[32];
	uint32_t index;

    virtual void unpack_hex(std::stringstream &ss);
	virtual int pack(char *output, int &output_len);
    virtual int pack_hex(std::stringstream &ss);
    virtual bool is_valid();

	Outpoint();
	virtual ~Outpoint();
};

