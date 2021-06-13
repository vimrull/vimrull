#include <cstdint>
#include <cstring>
#include "Outpoint.h"
#include "load.h"


Outpoint::Outpoint()
{
}

Outpoint::~Outpoint()
{
}

void Outpoint::unpack(std::stringstream &ss)
{
    read(ss, 32, hash);
    index = read_int32(ss);
}

int Outpoint::pack(char *output, int &output_len)
{
	char *pos = output;
	std::memcpy(pos, hash, 32);
	pos += 32;
	output_len -= 32;

	std::memcpy(pos, &index, sizeof(uint32_t));
	pos += sizeof(uint32_t);
	output_len -= sizeof(uint32_t);

	return int(pos - output);
}

int Outpoint::pack(std::stringstream &ss)
{
    int pos = 0;
    pos += pack_hex(ss, hash, 32);
    pos += pack_hex(ss, &index, sizeof(uint32_t));
    return pos;
}
