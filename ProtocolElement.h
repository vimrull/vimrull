#pragma once
#include <iostream>
#include <sstream>

class ProtocolElement
{
public:
	ProtocolElement();
	virtual ~ProtocolElement();
	virtual void unpack(std::stringstream &ss) = 0;
	virtual int pack(std::stringstream &ss) = 0;
};

