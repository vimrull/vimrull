#include <tests/catch2.hpp>
#include <secp256k1.h>
#include "secp256k1_preallocated.h"

// pre-req: install https://github.com/vimrull/secp256k1 using following commands
// git clone https://github.com/vimrull/secp256k1
// cd secp256k1
// ./autogen.sh
// ./configure
// make
// sudo install

typedef struct {
    uint32_t s[8];
    uint32_t buf[16]; /* In big endian */
    size_t bytes;
} secp256k1_sha256;


static constexpr unsigned int SIZE                   = 65;
static constexpr unsigned int COMPRESSED_SIZE        = 33;
static constexpr unsigned int SIGNATURE_SIZE         = 72;
static constexpr unsigned int COMPACT_SIGNATURE_SIZE = 65;
unsigned int static GetLen(unsigned char chHeader)
{
    if (chHeader == 2 || chHeader == 3)
        return COMPRESSED_SIZE;
    if (chHeader == 4 || chHeader == 6 || chHeader == 7)
        return SIZE;
    return 0;
}

TEST_CASE("Test EcdsaP256 Signature: sign/verify using libsecp256k1", "[libsecp256k1]")
{
    /*
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    REQUIRE(ctx);
    secp256k1_pubkey pubkey;
    secp256k1_ecdsa_signature sig;

    unsigned char vch[SIZE];
    if (!secp256k1_ec_pubkey_parse(ctx, &pubkey, vch, GetLen(vch[0]))) {
        FAIL();
    }
    secp256k1_context_destroy(ctx);
     */
}