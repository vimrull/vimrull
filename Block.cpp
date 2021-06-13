#include "Block.h"

unsigned char *dhash(unsigned char *data, long len);

unsigned char *calculate_merkle_root(std::vector<unsigned char *> *txhashes);

Block::Block()
{
}

Block::~Block()
{
}

void Block::unpack(std::stringstream &ss)
{
    this->header.unpack(ss);
    this->transaction_count = read_var_int(ss);

    for (int i = 0; i < transaction_count; i++)
    {
        //std::cout << "Loading transaction " << i << std::endl;
        //std::cout << "Before tx: " << ss.tellg() << std::endl;
        Transaction tx;
        tx.unpack(ss);
        this->transactions.push_back(tx);
        //std::cout << "After tx: " << ss.tellg() << std::endl;
        //std::cout << "TX lock_time " << tx.locktime << std::endl;
        //std::cout << "Previous output of tx input 0" << tx.inputs[0].previous_output.index << std::endl;
        //std::cout << "Stream at: " << ss.tellg() << std::endl;
    }
}

int Block::pack(std::stringstream &ss)
{
    header.pack(ss);
    pack_var_int(ss, transaction_count);
    for (auto tx: transactions)
    {
        tx.pack(ss);
    }

    return 0;
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
        tx.pack(ss);
        auto hash = dhash((unsigned char *) ss.str().c_str(), 1024 * 1024 - len);
        trans.push_back(hash);
        hexdump(hash, 32);
    }

    std::cout << std::dec << trans.size() << std::endl;

    std::cout << "------------- Merkle Root ----------\n";
    auto mr = calculate_merkle_root(&trans);
    return mr;
}
