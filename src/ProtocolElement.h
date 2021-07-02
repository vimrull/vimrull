#pragma once
#include <iostream>
#include <sstream>

class ProtocolElement
{
public:
	ProtocolElement();
	virtual ~ProtocolElement();
	virtual void unpack_hex(std::stringstream &ss) = 0;
	virtual int pack_hex(std::stringstream &ss) = 0;

    virtual bool is_valid() = 0;
};

