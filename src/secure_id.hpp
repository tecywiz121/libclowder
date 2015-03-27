/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_SECURE_ID_HPP
#define CLOWDER_SECURE_ID_HPP

#include "state.hpp"

#include <clowder/common.hpp>
#include <clowder/node_id.hpp>
#include <clowder/address.hpp>

#include <memory>

#include <botan/auto_rng.h>
#include <botan/ecdsa.h>
#include <botan/x509self.h>

namespace clowder
{

class secure_id : public node_id, public uses_botan
{
public:
    secure_id();

    secure_id(const secure_id&) = default;
    secure_id(secure_id&&) = default;
    secure_id(const std::vector<uint8_t>&);
    secure_id(const address&);

    secure_id& operator=(const secure_id&) = default;
    secure_id& operator=(secure_id&&) = default;

private:
    Botan::AutoSeeded_RNG _rng;
    std::unique_ptr<Botan::ECDSA_PrivateKey> _key;
    Botan::X509_Certificate _cert;

    std::unique_ptr<Botan::ECDSA_PrivateKey> generate_key();
    Botan::X509_Certificate generate_x509_certificate(const Botan::Private_Key&);
    static std::string fingerprint(const Botan::X509_Certificate&);
};

}

#endif /* CLOWDER_SECURE_ID_HPP */
