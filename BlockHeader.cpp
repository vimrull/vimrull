#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "BlockHeader.h"
#include "load.h"

BlockHeader::BlockHeader()
{
}


BlockHeader::~BlockHeader()
{
}

void BlockHeader::unpack(std::stringstream &ss) {
	char data[65];
	ss.read(data, 8);
	hex2bin((unsigned char *)&version, (unsigned char *)data, 8);

	ss.read(data, 32 * 2);
	//byte_swap((unsigned char *)data, 64);
	hex2bin((unsigned char *)&prev_block, (unsigned char *)data, 32 * 2);

	ss.read(data, 32 * 2);
	//byte_swap((unsigned char *)data, 64);
	hex2bin((unsigned char *)&merkle_root, (unsigned char *)data, 32 * 2);

	ss.read(data, 4 * 2);
	hex2bin((unsigned char *)&timestamp, (unsigned char *)data, 4 * 2);

	ss.read(data, 4 * 2);
	hex2bin((unsigned char *)&bits, (unsigned char *)data, 4 * 2);

	ss.read(data, 4 * 2);
	hex2bin((unsigned char *)&nonce, (unsigned char *)data, 4 * 2);
}

int BlockHeader::pack(std::stringstream &ss)
{
    int pos = 0;
    pos += pack_ptr2hex(ss, &version, sizeof(int32_t));
    pos += pack_ptr2hex(ss, &prev_block, 32);
    pos += pack_ptr2hex(ss, &merkle_root, 32);
    pos += pack_ptr2hex(ss, &timestamp, sizeof(uint32_t));
    pos += pack_ptr2hex(ss, &bits, sizeof(uint32_t));
    pos += pack_ptr2hex(ss, &nonce, sizeof(uint32_t));
	return pos;
}

bool BlockHeader::is_valid()
{
    return true;
}
