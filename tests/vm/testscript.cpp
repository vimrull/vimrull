#include <catch2/catch2.hpp>
#include <vm/opcodes.h>
#include <vm/variable.h>
#include <vm/BitcoinVM.h>
#include <Block.h>
#include <hashlib/sha256.h>
#include <cstring>
#include "load.h"
#include "crypto/util.h"

TEST_CASE("Test standard script", "[vm]")
{

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

    // 0x41 <0x41 bytes data> OP_CHECKSIG
    std::string op_codes_str="410411db93e1dcdb8a016b49840f8c53bc1eb68a382e97b1482ecad7b148a6909a5cb2e0eadd"
                             "fb84ccf9744464f82e160bfa9b8b64f9d4c03f999b8643f656b412a3ac";

    std::vector<unsigned char> op_codes;
    op_codes.resize(op_codes_str.length() / 2);
    hex2bin((unsigned char *)op_codes.data(), (unsigned char *)op_codes_str.c_str(), op_codes_str.length());

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

    // std::cout <<script_str << std::endl;
    // first byte is script length and last byte is hash type - so we subtract 2
    std::vector<unsigned char> signature(output_block.transactions[1].inputs[0].script_size-2);
    int script_len = output_block.transactions[1].inputs[0].script[0];
    std::memcpy(signature.data(), &output_block.transactions[1].inputs[0].script[1], script_len-1);
    REQUIRE(output_block.transactions[1].inputs[0].script[output_block.transactions[1].inputs[0].script_size-1] == 0x01);
    REQUIRE(signature.size() == 70);

    u_int32_t hash_type = output_block.transactions[1].inputs[0].script[
                            output_block.transactions[1].inputs[0].script_size-1];

    REQUIRE(hash_type == SIGHASH_ALL);
    std::stringstream tx1ss;
    // this destroys the original block - TODO: fix that using vector like
    //std::vector<unsigned char> locking_script(input_block.transactions[0].outputs[0].script,
    //                                          input_block.transactions[0].outputs[0].script
    //                                          + input_block.transactions[0].outputs[0].script_size);
    output_block.transactions[1].inputs[0].script_size = input_block.transactions[0].outputs[0].script_size;
    output_block.transactions[1].inputs[0].script = input_block.transactions[0].outputs[0].script;

    output_block.transactions[1].pack_hex(tx1ss);
    auto hash_type_str = hexdump((void *) &hash_type, 4); //"01000000";
    std::string raw_tx_hex = tx1ss.str() + hash_type_str;
    std::vector<unsigned char> raw_tx(raw_tx_hex.length()/2);
    hex2bin(raw_tx.data(), (unsigned char *)raw_tx_hex.c_str(), raw_tx_hex.length());

    //TODO: figure out if we need to resize as we initialize vector with len
    raw_tx.resize(raw_tx_hex.length()/2);

    long tlen0 = raw_tx.size();
    auto hashtx_raw0 =  dhash((unsigned char *)raw_tx.data(), tlen0);
    std::vector<unsigned char> hashtx0(hashtx_raw0, hashtx_raw0 + 32);
    std::string tx_hash0 = hexdump((void*) hashtx0.data(), 32);
    REQUIRE(tx_hash0.compare("7a05c6145f10101e9d6325494245adf1297d80f8f38d4d576d57cdba220bcb19") == 0);

    REQUIRE(valid_signature(hashtx0, signature, script));

    // vm test

    auto vm = std::make_unique<BitcoinVM>();
    variable v_sig, v;
    v_sig.type_ = value_type::DATA;
    v_sig.data = signature;
    vm->stack_.push(v_sig);
    v.type_ = value_type::DATA;
    v.data = hashtx0;
    vm->stack_.push(v);
    REQUIRE(vm->execute(op_codes));
}
