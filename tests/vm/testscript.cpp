#include <tests/catch2.hpp>
#include <vm/opcodes.h>
#include <vm/variable.h>
#include <vm/BitcoinVM.h>
#include <Block.h>
#include <hashlib/sha256.h>
#include "load.h"
#include "crypto/util.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/osrng.h"
#include "cryptopp/oids.h"
#include <cryptopp/hex.h>
using namespace CryptoPP;

TEST_CASE("Test standard script", "[vm]")
{
    // TODO: put actual address and signature
    // OP_DUP OP_HASH160 <bytelen> <bytelen bytes> OP_EQUALVERIFY OP_CHECKSIG
    std::string op_codes_str="76A914AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA88AC";
    std::vector<unsigned char> op_codes;
    op_codes.resize(op_codes_str.length() / 2 + 1);
    hex2bin((unsigned char *)op_codes.data(), (unsigned char *)op_codes_str.c_str(), op_codes_str.length());
    auto vm = new BitcoinVM();// std::make_unique<BitcoinVM>();
    variable v_sig, v;
    v_sig.type_ = value_type::DATA;
    v_sig.data = {'a', 'b', 'c'};
    vm->stack_.push(v_sig);
    v.type_ = value_type::STRING;
    v.str = "hello";
    vm->stack_.push(v);
    REQUIRE(vm->execute(op_codes));
    delete vm;
}

TEST_CASE("Validate transaction in block 170", "[transaction]")
{
    // In block 170 there are two transaction outputs that split 50 bitcoints from block 9 transaction. The
    // 50 coins are distributed between two outputs by 10 and 40 bitcoins.

    std::string input_block_id = "000000008d9dc510f23c2657fc4f67bea30078cc05a90eb89e84cc475c080805"; // block 9
    std::string input_block_filename = "bitcoindata/rawblock/" + input_block_id + ".hex";
    std::stringstream input_block_stream;
    REQUIRE(read_block_file(input_block_filename, input_block_stream));

    Block input_block;
    input_block.unpack_hex(input_block_stream);
    REQUIRE(input_block.transactions[0].output_count == 1);

    std::stringstream  ss_input_output_tx;
    input_block.transactions[0].pack_hex(ss_input_output_tx);
    std::string input_output_tx = ss_input_output_tx.str();
    std::vector<unsigned char> input_output_tx_bin(input_output_tx.length()/2+1);
    hex2bin(input_output_tx_bin.data(), (unsigned char *) input_output_tx.c_str(), input_output_tx.length());
    input_output_tx_bin.resize(input_output_tx.length()/2);

    long input_tx_len = input_output_tx_bin.size();
    auto hash_input_tx = dhash((unsigned char *)(input_output_tx_bin.data()), input_tx_len);
    hash_input_tx[32] = 0;
    std::string hash_input_tx_str =  hexdump((void *)hash_input_tx, 32);

    REQUIRE(input_block.transactions[0].outputs[0].script_size == 0x43);
    REQUIRE(input_block.transactions[0].outputs[0].script[0] == 0x41);
    auto locking_script_size = input_block.transactions[0].outputs[0].script_size;
    std::string locking_script_hex = hexdump((void*)input_block.transactions[0].outputs[0].script, locking_script_size);
    std::string expected_locking_script = "410411db93e1dcdb8a016b49840f8c53bc1eb68a382e97b1482ecad7b148a6909a5cb2e0eadd"
                                          "fb84ccf9744464f82e160bfa9b8b64f9d4c03f999b8643f656b412a3ac";
    REQUIRE(locking_script_hex.compare(expected_locking_script) == 0);
    // output
    std::string block_id = "00000000d1145790a8694403d4063f323d499e655c83426834d4ce2f8dd4a2ee"; // block 170
    std::string block_filename = "bitcoindata/rawblock/" + block_id + ".hex";
    std::stringstream block_stream;
    REQUIRE(read_block_file(block_filename, block_stream));
    std::string block_str = block_stream.str();

    Block output_block;
    output_block.unpack_hex(block_stream);
    std::stringstream ss1;
    output_block.pack_hex(ss1);
    std::string packed_str = ss1.str();
    REQUIRE(block_str.compare(packed_str) == 0);
    REQUIRE(output_block.transaction_count == 2);
    REQUIRE(output_block.transactions[0].input_count == 1);
    REQUIRE(output_block.transactions[0].inputs[0].IsCoinbase() == true);
    REQUIRE(output_block.transactions[1].input_count == 1);
    REQUIRE(output_block.transactions[1].inputs[0].IsCoinbase() == false);
    REQUIRE(input_block.transactions[0].inputs[0].sequence == 0xffffffff);

    std::string expected_tx_input = "c997a5e56e104102fa209c6a852dd90660a20b2d9c352423edce25857fcd3704000000004847304402"
                                    "204e45e16932b8af514961a1d3a1a25fdf3f4f7732e9d624c6c61548ab5fb8cd410220181522ec8eca"
                                    "07de4860a4acdd12909d831cc56cbbac4622082221a8768d1d0901ffffffff";
    std::stringstream tx_input;
    output_block.transactions[1].inputs[0].pack_hex(tx_input);
    std::string tx_input_str = tx_input.str();

    REQUIRE(expected_tx_input.compare(tx_input_str) == 0);
    std::string expect_previous_tx_hash = "c997a5e56e104102fa209c6a852dd90660a20b2d9c352423edce25857fcd3704";

    std::string previous_tx_hash = hexdump((void *)output_block.transactions[1].inputs[0].previous_output.hash, 32);
    REQUIRE(expect_previous_tx_hash.compare(previous_tx_hash) == 0);
    REQUIRE(expect_previous_tx_hash.compare(hash_input_tx_str) == 0);
    REQUIRE(output_block.transactions[1].inputs[0].previous_output.index == 0);

    REQUIRE(input_block.transactions[0].outputs[output_block.transactions[1].inputs[0].previous_output.index].script_size == 0x43);
    REQUIRE(input_block.transactions[0].outputs[output_block.transactions[1].inputs[0].previous_output.index].value == 5000000000);
    auto input_script = input_block.transactions[0].outputs[output_block.transactions[1].inputs[0].previous_output.index].script;
    REQUIRE(input_script[0] == 0x41);
    REQUIRE(input_script[0+0x41+1] == OP_CHECKSIG);

    REQUIRE(output_block.transaction_count == 2);
    REQUIRE(output_block.transactions[0].output_count == 1);
    REQUIRE(output_block.transactions[1].output_count == 2);

    //output 0
    REQUIRE(output_block.transactions[1].outputs[0].value == 1000000000);
    REQUIRE(output_block.transactions[1].outputs[0].script_size == 0x43);
    auto output_script0 = output_block.transactions[1].outputs[0].script;
    REQUIRE(output_script0[0] == 0x41);
    REQUIRE(output_script0[0+0x41+1] == OP_CHECKSIG);

    // output 1
    REQUIRE(output_block.transactions[1].outputs[1].value == 4000000000);
    REQUIRE(output_block.transactions[1].outputs[1].script_size == 0x43);
    auto output_script1 = output_block.transactions[1].outputs[1].script;
    REQUIRE(output_script1[0] == 0x41);
    REQUIRE(output_script1[0+0x41+1] == OP_CHECKSIG);
    int slen = output_script1[0];
    std::vector<unsigned char> script(slen);
    std::memcpy(script.data(), &output_script1[1], slen);
    std::string script_str = hexdump((void*)script.data(), slen);

    std::vector<unsigned char> expected_signature = {0x30, 0x44, 0x02, 0x20, 0x4e, 0x45, 0xe1, 0x69, 0x32, 0xb8, 0xaf,
                                                     0x51, 0x49, 0x61, 0xa1, 0xd3, 0xa1, 0xa2, 0x5f, 0xdf, 0x3f, 0x4f,
                                                     0x77, 0x32, 0xe9, 0xd6, 0x24, 0xc6, 0xc6, 0x15, 0x48, 0xab, 0x5f,
                                                     0xb8, 0xcd, 0x41, 0x02, 0x20, 0x18, 0x15, 0x22, 0xec, 0x8e, 0xca,
                                                     0x07, 0xde, 0x48, 0x60, 0xa4, 0xac, 0xdd, 0x12, 0x90, 0x9d, 0x83,
                                                     0x1c, 0xc5, 0x6c, 0xbb, 0xac, 0x46, 0x22, 0x08, 0x22, 0x21, 0xa8,
                                                     0x76, 0x8d, 0x1d, 0x09 };

    std::vector<byte> expected_public_key = {0x04, 0x11, 0xdb, 0x93, 0xe1, 0xdc, 0xdb, 0x8a, 0x01, 0x6b, 0x49, 0x84,
                                             0x0f, 0x8c, 0x53, 0xbc, 0x1e, 0xb6, 0x8a, 0x38, 0x2e, 0x97, 0xb1, 0x48,
                                             0x2e, 0xca, 0xd7, 0xb1, 0x48, 0xa6, 0x90, 0x9a, 0x5c, 0xb2, 0xe0, 0xea,
                                             0xdd, 0xfb, 0x84, 0xcc, 0xf9, 0x74, 0x44, 0x64, 0xf8, 0x2e, 0x16, 0x0b,
                                             0xfa, 0x9b, 0x8b, 0x64, 0xf9, 0xd4, 0xc0, 0x3f, 0x99, 0x9b, 0x86, 0x43,
                                             0xf6, 0x56, 0xb4, 0x12, 0xa3};

    std::vector<unsigned char> expected_raw_tx = {0x01, 0x00, 0x00, 0x00, 0x01, 0xc9, 0x97, 0xa5, 0xe5, 0x6e, 0x10,
                                                  0x41, 0x02, 0xfa, 0x20, 0x9c, 0x6a, 0x85, 0x2d, 0xd9, 0x06, 0x60,
                                                  0xa2, 0x0b, 0x2d, 0x9c, 0x35, 0x24, 0x23, 0xed, 0xce, 0x25, 0x85,
                                                  0x7f, 0xcd, 0x37, 0x04, 0x00, 0x00, 0x00, 0x00, 0x43, 0x41, 0x04,
                                                  0x11, 0xdb, 0x93, 0xe1, 0xdc, 0xdb, 0x8a, 0x01, 0x6b, 0x49, 0x84,
                                                  0x0f, 0x8c, 0x53, 0xbc, 0x1e, 0xb6, 0x8a, 0x38, 0x2e, 0x97, 0xb1,
                                                  0x48, 0x2e, 0xca, 0xd7, 0xb1, 0x48, 0xa6, 0x90, 0x9a, 0x5c, 0xb2,
                                                  0xe0, 0xea, 0xdd, 0xfb, 0x84, 0xcc, 0xf9, 0x74, 0x44, 0x64, 0xf8,
                                                  0x2e, 0x16, 0x0b, 0xfa, 0x9b, 0x8b, 0x64, 0xf9, 0xd4, 0xc0, 0x3f,
                                                  0x99, 0x9b, 0x86, 0x43, 0xf6, 0x56, 0xb4, 0x12, 0xa3, 0xac, 0xff,
                                                  0xff, 0xff, 0xff, 0x02, 0x00, 0xca, 0x9a, 0x3b, 0x00, 0x00, 0x00,
                                                  0x00, 0x43, 0x41, 0x04, 0xae, 0x1a, 0x62, 0xfe, 0x09, 0xc5, 0xf5,
                                                  0x1b, 0x13, 0x90, 0x5f, 0x07, 0xf0, 0x6b, 0x99, 0xa2, 0xf7, 0x15,
                                                  0x9b, 0x22, 0x25, 0xf3, 0x74, 0xcd, 0x37, 0x8d, 0x71, 0x30, 0x2f,
                                                  0xa2, 0x84, 0x14, 0xe7, 0xaa, 0xb3, 0x73, 0x97, 0xf5, 0x54, 0xa7,
                                                  0xdf, 0x5f, 0x14, 0x2c, 0x21, 0xc1, 0xb7, 0x30, 0x3b, 0x8a, 0x06,
                                                  0x26, 0xf1, 0xba, 0xde, 0xd5, 0xc7, 0x2a, 0x70, 0x4f, 0x7e, 0x6c,
                                                  0xd8, 0x4c, 0xac, 0x00, 0x28, 0x6b, 0xee, 0x00, 0x00, 0x00, 0x00,
                                                  0x43, 0x41, 0x04, 0x11, 0xdb, 0x93, 0xe1, 0xdc, 0xdb, 0x8a, 0x01,
                                                  0x6b, 0x49, 0x84, 0x0f, 0x8c, 0x53, 0xbc, 0x1e, 0xb6, 0x8a, 0x38,
                                                  0x2e, 0x97, 0xb1, 0x48, 0x2e, 0xca, 0xd7, 0xb1, 0x48, 0xa6, 0x90,
                                                  0x9a, 0x5c, 0xb2, 0xe0, 0xea, 0xdd, 0xfb, 0x84, 0xcc, 0xf9, 0x74,
                                                  0x44, 0x64, 0xf8, 0x2e, 0x16, 0x0b, 0xfa, 0x9b, 0x8b, 0x64, 0xf9,
                                                  0xd4, 0xc0, 0x3f, 0x99, 0x9b, 0x86, 0x43, 0xf6, 0x56, 0xb4, 0x12,
                                                  0xa3, 0xac, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    //std::cout << "Tx that works: " << hexdump((void *) expected_raw_tx.data(), expected_raw_tx.size()) << std::endl;
    std::string expected_public_key_hex = hexdump((void *) expected_public_key.data(), expected_public_key.size());
    REQUIRE(expected_public_key_hex.compare(script_str) == 0);

    // first byte is script length and last byte is hash type - so we subtract 2
    std::vector<unsigned char> signature(output_block.transactions[1].inputs[0].script_size-2);
    int script_len = output_block.transactions[1].inputs[0].script[0];
    std::memcpy(signature.data(), &output_block.transactions[1].inputs[0].script[1], script_len-1);
    REQUIRE(output_block.transactions[1].inputs[0].script[output_block.transactions[1].inputs[0].script_size-1] == 0x01);
    REQUIRE(signature.size() == 70);
    REQUIRE(expected_signature == signature);
    //std::vector<unsigned char> hashtx(32);

    // From the actual block
    std::stringstream tx1ss;
    std::vector<unsigned char> locking_script(input_block.transactions[0].outputs[0].script,
                                              input_block.transactions[0].outputs[0].script
                                              + input_block.transactions[0].outputs[0].script_size);

    output_block.transactions[1].inputs[0].script_size = input_block.transactions[0].outputs[0].script_size;
    output_block.transactions[1].inputs[0].script = input_block.transactions[0].outputs[0].script;

    output_block.transactions[1].pack_hex(tx1ss);
    std::string raw_tx_hex = tx1ss.str() + "01000000";
    //std::cout << "From block: "  << raw_tx_hex << std::endl;
    std::vector<byte> raw_tx(raw_tx_hex.length()/2);
    hex2bin(raw_tx.data(), (unsigned char *)raw_tx_hex.c_str(), raw_tx_hex.length());
    REQUIRE(expected_raw_tx == raw_tx);
    std::cout << raw_tx_hex << std::endl;

    //TODO: figure out if we need to resize as we initialize vector with len
    raw_tx.resize(raw_tx_hex.length()/2);

    long tlen0 = raw_tx.size();
    auto hashtx_raw0 =  dhash((unsigned char *)raw_tx.data(), tlen0);
    std::vector<unsigned char> hashtx0(hashtx_raw0, hashtx_raw0 + 32);
    std::string tx_hash0 = hexdump((void*) hashtx0.data(), 32);
    REQUIRE(tx_hash0.compare("7a05c6145f10101e9d6325494245adf1297d80f8f38d4d576d57cdba220bcb19") == 0);
    REQUIRE(valid_signature(hashtx0, signature, script));
}
