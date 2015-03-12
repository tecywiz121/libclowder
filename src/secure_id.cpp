/* vim: et sw=4 sts=4 ts=4 : */
#include "secure_id.hpp"

#include "compat.hpp"

#include <botan/build.h>
#include <botan/sha2_32.h>
#include <botan/crc32.h>
#include <botan/hex.h>

using Botan::AutoSeeded_RNG;
using Botan::EC_Group;
using Botan::Private_Key;
using Botan::ECDSA_PrivateKey;
using Botan::X509_Certificate;
using Botan::X509_Cert_Options;
using std::unique_ptr;
using std::vector;

namespace clowder
{

secure_id::secure_id(const address& addr)
    : secure_id(addr.to_bytes())
{

}

secure_id::secure_id()
    : secure_id(vector<uint8_t>())
{

}

secure_id::secure_id(const vector<uint8_t>& addr)
    : _key(generate_key()),
      _cert(generate_x509_certificate(*_key))
{
    Botan::CRC32 hash;
    hash.update(addr.data(), addr.size());
    std::string crc32 = Botan::hex_encode(hash.final());
    set(crc32 + fingerprint(_cert));
}

unique_ptr<ECDSA_PrivateKey> secure_id::generate_key()
{
    EC_Group group("secp256r1");

    unique_ptr<ECDSA_PrivateKey> priv = make_unique<ECDSA_PrivateKey>(_rng, group);
    if (!priv->check_key(_rng, true)) {
        throw std::runtime_error("problem checking key");
    }

    return priv;
}

X509_Certificate secure_id::generate_x509_certificate(const Private_Key& key)
{
    X509_Cert_Options opts;
    opts.country = "XZ";
    opts.common_name = "clowder";

    X509_Certificate cert = Botan::X509::create_self_signed_cert(opts,
                                                                 key,
                                                                 "SHA-256",
                                                                 _rng);
    return cert;
}

std::string secure_id::fingerprint(const X509_Certificate& cert)
{
    Botan::SHA_224 hash;
    hash.update(cert.BER_encode());
    return Botan::hex_encode(hash.final());
}

}
