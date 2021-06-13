#include "Block.h"

unsigned char *dhash(unsigned char *data, long len);

unsigned char *calculate_merkle_root(std::vector<unsigned char *> *txhashes);

Block::Block()
{
}

Block::~Block()
{
}

void Block::unpack_hex(std::stringstream &ss)
{
    this->header.unpack(ss);
    this->transaction_count = read_var_int(ss);

    for (int i = 0; i < transaction_count; i++)
    {
        Transaction tx;
        tx.unpack_hex(ss);
        this->transactions.push_back(tx);
    }
}

int Block::pack_hex(std::stringstream &ss)
{
    header.pack(ss);
    pack_var_int(ss, transaction_count);
    for (auto tx: transactions)
    {
        tx.pack_hex(ss);
    }

    return 0;
}

bool Block::is_valid()
{
    ENSURE_IS_VALID (header.is_valid());
    ENSURE_IS_VALID (transaction_count >= 1);
    ENSURE_IS_VALID (!transactions.empty() || transactions.size() == transaction_count);
    for (auto& tx: transactions)
    {
        ENSURE_IS_VALID (tx.is_valid());
    }

    return true;
}

unsigned char *Block::CalculateMerkleRoot()
{
    std::vector<unsigned char *> trans;
    char *txd = (char *) malloc(1024 * 1024);
    for (int i = 0; i < transaction_count; i++)
    {
        auto tx = transactions[i];

        int len = 1024 * 1024;
        std::stringstream ss;
        tx.pack_hex(ss);
        auto hash = dhash((unsigned char *) ss.str().c_str(), 1024 * 1024 - len);
        trans.push_back(hash);
        hexdump(hash, 32);
    }

    std::cout << std::dec << trans.size() << std::endl;

    std::cout << "------------- Merkle Root ----------\n";
    auto mr = calculate_merkle_root(&trans);
    return mr;
}
