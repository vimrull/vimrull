
#include <tests/catch2.hpp>
#include <vm/opcodes.h>
#include <vm/variable.h>
#include <vm/BitcoinVM.h>
#include <Block.h>
#include <hashlib/sha256.h>
#include "load.h"

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

    //TODO: get it from the block
    std::string pt = "0411db93e1dcdb8a016b49840f8c53bc"
                            "1eb68a382e97b1482ecad7b148a6909a"
                            "5cb2e0eaddfb84ccf9744464f82e160b"
                            "fa9b8b64f9d4c03f999b8643f656b412a3";

    HexDecoder decoder3;
    decoder3.Put((byte*)pt.c_str(), pt.size());
    decoder3.MessageEnd();

    ECP::Point q;
    size_t len = decoder3.MaxRetrievable();

    q.identity = false;
    q.x.Decode(decoder3, len/2);
    q.y.Decode(decoder3, len/2);

    ECDSA<ECP, SHA256>::PublicKey publicKey;
    publicKey.Initialize( ASN1::secp256r1(), q );

    AutoSeededRandomPool prng;
    publicKey.Validate(prng, 70);
    ECDSA<ECP, SHA256>::Verifier verifier(publicKey);

    //TODO: verify signature
}
