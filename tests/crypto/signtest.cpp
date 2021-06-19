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

#include <iostream>
#include <load.h>

// Adapted from google/tink example.
// For details see: https://github.com/google/tink/blob/master/examples/cc/digital_signatures/digital_signatures_cli.cc
TEST_CASE("Test EcdsaP256 Signature: sign/verify", "[crypto]")
{
    auto status = crypto::tink::TinkConfig::Register();
    REQUIRE(status.ok());

    auto key_template = crypto::tink::SignatureKeyTemplates::EcdsaP256();
    auto new_keyset_handle_result = crypto::tink::KeysetHandle::GenerateNew(key_template);
    if (!new_keyset_handle_result.ok()) {
        std::clog << "Generating new keyset failed: "
                  << new_keyset_handle_result.status().error_message()
                  << std::endl;
        FAIL();
    }

    auto keyset_handle = std::move(new_keyset_handle_result.ValueOrDie());

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
