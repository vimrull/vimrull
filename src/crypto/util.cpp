#include <openssl/obj_mac.h>
#include <openssl/ossl_typ.h>
#include <openssl/ec.h>
#include <memory>
#include <functional>
#include "util.h"

bool valid_signature(std::vector<unsigned char> data, std::vector<unsigned char> sig, std::vector<unsigned char> pubkey)
{
    std::unique_ptr<EC_KEY, std::function<void(EC_KEY*)>> key_ptr(EC_KEY_new_by_curve_name(NID_secp256k1),
                                                                  [](EC_KEY* k)
    {
        EC_KEY_free(k);
    });

    uint8_t *pubkey_data = pubkey.data();
    auto key = key_ptr.get();
    if (nullptr != o2i_ECPublicKey(&key, (const uint8_t **)(&pubkey_data), pubkey.size())) {
        return (ECDSA_verify(0, (const unsigned char *) (data.data()), data.size(),
                             sig.data(), sig.size(), key) == 1);
    }

    return false;
}
