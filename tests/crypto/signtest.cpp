#include <tests/catch2.hpp>
#include <tink/keyset_handle.h>
#include "tink/signature/signature_key_templates.h"
#include "tink/public_key_sign.h"
#include "tink/public_key_verify.h"
#include "tink/config/tink_config.h"

#include "tink/binary_keyset_reader.h"
#include "tink/binary_keyset_writer.h"
#include "tink/cleartext_keyset_handle.h"
#include "tink/keyset_reader.h"
#include "tink/keyset_writer.h"
#include "tink/config.h"
#include "tink/keyset_handle.h"
#include "tink/config/tink_config.h"
#include "tink/util/status.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "tink/keyset_handle.h"
#include "tink/keyset_reader.h"
#include "tink/public_key_sign.h"
#include "tink/public_key_verify.h"
#include "tink/signature/rsa_ssa_pss_sign_key_manager.h"
#include "tink/signature/rsa_ssa_pss_verify_key_manager.h"
#include "tink/signature/signature_config.h"
#include "tink/subtle/pem_parser_boringssl.h"
#include "tink/subtle/subtle_util_boringssl.h"
#include "tink/util/enums.h"
#include "tink/util/secret_data.h"
#include "tink/util/status.h"

#include <iostream>
#include <load.h>
#include <proto/tink.pb.h>
#include <tink/signature/signature_pem_keyset_reader.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>

// install
// git clone https://github.com/vimrull/cryptopp
// cd cryptocpp
// make && sudo make install
#include "cryptopp/eccrypto.h"
#include "cryptopp/osrng.h"
#include "cryptopp/oids.h"
// static
std::unique_ptr<crypto::tink::KeysetWriter> GetBinaryKeysetWriter(
        const std::string& filename) {
    std::unique_ptr<std::ofstream> keyset_stream(new std::ofstream());
    keyset_stream->open(filename, std::ofstream::out);
    auto keyset_writer_result = crypto::tink::BinaryKeysetWriter::New(std::move(keyset_stream));
    if (!keyset_writer_result.ok()) {
        std::clog << "Creation of the BinaryKeysetWriter failed: "
                  << keyset_writer_result.status().error_message() << std::endl;
        exit(1);
    }
    return std::move(keyset_writer_result.ValueOrDie());
}

void WriteKeyset(const std::unique_ptr<crypto::tink::KeysetHandle>& keyset_handle,
                       const std::string& filename) {
    auto keyset_writer = GetBinaryKeysetWriter(filename);
    auto status = crypto::tink::CleartextKeysetHandle::Write(keyset_writer.get(),
                                               *keyset_handle);
    if (!status.ok()) {
        std::clog << "Writing the keyset failed: "
                  << status.error_message() << std::endl;
        exit(1);
    }
}
// Adapted from google/tink example.
// For details see: https://github.com/google/tink/blob/master/examples/cc/digital_signatures/digital_signatures_cli.cc
TEST_CASE("Test EcdsaP256 Signature: sign/verify", "[crypto]")
{
    auto status = crypto::tink::TinkConfig::Register();
    REQUIRE(status.ok());

    auto key_template = crypto::tink::SignatureKeyTemplates::EcdsaP256();
    key_template.set_output_prefix_type(google::crypto::tink::OutputPrefixType::TINK);

    auto new_keyset_handle_result = crypto::tink::KeysetHandle::GenerateNew(key_template);
    if (!new_keyset_handle_result.ok()) {
        std::clog << "Generating new keyset failed: "
                  << new_keyset_handle_result.status().error_message()
                  << std::endl;
        FAIL();
    }

    auto keyset_handle = std::move(new_keyset_handle_result.ValueOrDie());

    //WriteKeyset(keyset_handle, "key.bin");
    auto primitive_result = keyset_handle->GetPrimitive<crypto::tink::PublicKeySign>();
    REQUIRE(primitive_result.ok());

    auto public_key_sign = std::move(primitive_result.ValueOrDie());

    std::string message = "test message";

    auto sign_result = public_key_sign->Sign(message);
    if (!sign_result.ok()) {
        std::cout << "Error while signing the message: "
                  << sign_result.status().error_message() << std::endl;
    }
    REQUIRE(sign_result.ok());

    std::string signature = sign_result.ValueOrDie();
    //std::cout << hexdump((char *)signature.c_str(), signature.length()) << std::endl;

    // verify signature
    auto new_keyset_handle_verify =
            keyset_handle->GetPublicKeysetHandle();
    if (!new_keyset_handle_verify.ok()) {
        std::clog << "Getting the keyset failed: "
                  << new_keyset_handle_verify.status().error_message()
                  << std::endl;
        exit(1);
    }

    auto public_keyset_handle =
            std::move(new_keyset_handle_verify.ValueOrDie());

    auto primitive_result_verify =
            public_keyset_handle->GetPrimitive<crypto::tink::PublicKeyVerify>();
    if (!primitive_result_verify.ok()) {
        std::clog << "Getting PublicKeyVerify-primitive from the factory "
                  << "failed: " << primitive_result_verify.status().error_message()
                  << std::endl;
        FAIL();
    }
    auto public_key_verify = std::move(primitive_result_verify.ValueOrDie());

    std::string result;
    auto verify_status = public_key_verify->Verify(signature, message);
    if (!verify_status.ok()) {
        std::clog << "Error while verifying the signature: "
                  << verify_status.error_message() << std::endl;
        FAIL();
    }
}

TEST_CASE("Test EC private key", "[crypto]")
{
    std::string key_hex="3074020101042081c70e36ffa5e3e6425dc19c7c35315d3d72dc60b79cb78fe009a335de29dd22a00706052b810400"
                        "0aa14403420004a2fef1829e0742b89c218c51898d9e7cb9d51201ba2bf9d9e9214ebb6af32708898688ee8b216fe6"
                        "78c8a0b4cd59d1fd3778d0b2ff06cd75d6b0ab39fbd80973";

    // hex2bin(unsigned char *dest, unsigned char *src, int len)
    std::vector<unsigned char> key;

    key.resize(key_hex.length()/2);
    key.reserve(key_hex.length()/2);
    hex2bin(key.data(), (unsigned char *) key_hex.c_str(), key_hex.length());

    auto status = crypto::tink::TinkConfig::Register();
    REQUIRE(status.ok());

}
TEST_CASE("Test PEM Format", "[crypto]")
{
    auto pem_key="-----BEGIN EC PRIVATE KEY-----\n"
                 "MHQCAQEEIIHHDjb/pePmQl3BnHw1MV09ctxgt5y3j+AJozXeKd0ioAcGBSuBBAAK\n"
                 "oUQDQgAEov7xgp4HQricIYxRiY2efLnVEgG6K/nZ6SFOu2rzJwiJhojuiyFv5njI\n"
                 "oLTNWdH9N3jQsv8GzXXWsKs5+9gJcw==\n"
                 "-----END EC PRIVATE KEY-----";
    auto builder = crypto::tink::SignaturePemKeysetReaderBuilder(
            crypto::tink::SignaturePemKeysetReaderBuilder::PemReaderType::PUBLIC_KEY_SIGN);

    crypto::tink::PemKey pem_serialized_key = {.serialized_key = std::string(pem_key),
            .parameters = {.key_type =  crypto::tink::PemKeyType::PEM_EC,
                    .algorithm = crypto::tink::PemAlgorithm::ECDSA,
                    .key_size_in_bits = 256,
                    .hash_type = google::crypto::tink::HashType::SHA256}};
    //builder.Add(pem_serialized_key);
    //auto keyset_reader_or = builder.Build();
    //REQUIRE(keyset_reader_or.status().ok());
}

TEST_CASE("Test crypto++ sign - generate key", "[sign]")
{
    //https://www.cryptopp.com/wiki/Elliptic_Curve_Digital_Signature_Algorithm
    using namespace CryptoPP;
    AutoSeededRandomPool prng;

    ECDSA<ECP, SHA256>::PrivateKey k1;
    k1.Initialize( prng, ASN1::secp256r1() );
    ECDSA<ECP, SHA256>::Signer signer(k1);

    std::string message = "Do or do not. There is no try.";
    size_t siglen = signer.MaxSignatureLength();
    std::string signature(siglen, 0x00);

    siglen = signer.SignMessage( prng, (const byte*)&message[0], message.size(), (byte*)&signature[0] );
    signature.resize(siglen);

    //std::cout << hexdump((void *) signature.c_str(), signature.length()) << std::endl;
    ECDSA<ECP, SHA256>::PublicKey publicKey;
    k1.MakePublicKey(publicKey);
    ECDSA<ECP, SHA256>::Verifier verifier(publicKey);

    bool result = verifier.VerifyMessage( (const byte*)&message[0], message.size(), (const byte*)&signature[0], signature.size() );

    // Verification failure?
    if( !result ) {
        //std::cout << "Failed to verify signature on message" << std::endl;
        FAIL();
    } else {
        //std::cout << "All good!" << std::endl;
    }
}

// https://www.cryptopp.com/wiki/Elliptic_Curve_Digital_Signature_Algorithm#Complete_Example
TEST_CASE("Test crypto++ sign - use existing key", "[sign]")
{
    using namespace CryptoPP;

    std::string keystr = "MHQCAQEEIIHHDjb/pePmQl3BnHw1MV09ctxgt5y3j+AJozXeKd0ioAcGBSuBBAAK\n"
                          "oUQDQgAEov7xgp4HQricIYxRiY2efLnVEgG6K/nZ6SFOu2rzJwiJhojuiyFv5njI\n"
                          "oLTNWdH9N3jQsv8GzXXWsKs5+9gJcw==";
    ByteQueue queue;
    Base64Decoder decoder;

    decoder.Attach(new Redirector(queue));
    decoder.Put((const byte*)keystr.data(), keystr.length());
    decoder.MessageEnd();
    AutoSeededRandomPool prng;

    ECDSA_RFC6979<ECP, SHA256>::PrivateKey k1;
    //k1.Initialize( prng, ASN1::secp256r1() );
    k1.BERDecodePrivateKey(queue, false /*paramsPresent*/, queue.MaxRetrievable());
    REQUIRE(queue.IsEmpty());

    AutoSeededRandomPool prng2;
    REQUIRE(k1.Validate(prng2, 2));
    ECDSA_RFC6979<ECP, SHA256>::Signer signer(k1);

    std::string message = "Do or do not. There is no try.";
    size_t siglen = signer.MaxSignatureLength();
    std::string signature(siglen, 0x00);

    siglen = signer.SignMessage( prng, (const byte*)message.c_str(), message.length(), (byte*)&signature[0] );
    signature.resize(siglen);
    std::string expected_sign_hex = "01c3f2dedfaa19e7f0ca9c2b96e3d3426f9a117694ddd4a3a72aeffdd585749b937b8f84eae6df6beb120a25f778199c4654ee0feee19a1d2387d5ae07e91ef8";
    std::string  sign_hex = hexdump((void *) signature.c_str(), signature.length());
    REQUIRE(expected_sign_hex.compare(sign_hex) == 0);
    ECDSA_RFC6979<ECP, SHA256>::PublicKey publicKey;
    k1.MakePublicKey(publicKey);
    ECDSA_RFC6979<ECP, SHA256>::Verifier verifier(publicKey);

    bool result = verifier.VerifyMessage( (const byte*)message.c_str(), message.size(), (const byte*)signature.c_str(), signature.size() );

    REQUIRE(result);
}