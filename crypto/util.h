#ifndef BITCOIN_UTIL_H
#define BITCOIN_UTIL_H
#include <vector>

bool valid_signature(std::vector<unsigned char> data, std::vector<unsigned char> sig, std::vector<unsigned char> pubkey);


#endif //BITCOIN_UTIL_H
