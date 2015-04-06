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
#include <botan/credentials_manager.h>

namespace clowder
{

class secure_id;

class secure_id_credentials_manager : public Botan::Credentials_Manager
{
private:
    secure_id& _id;

public:
    secure_id_credentials_manager(secure_id& id);
    virtual std::vector<Botan::X509_Certificate> cert_chain(
            const std::vector<std::string>&,    // TODO - SW: respect the request of the client
            const std::string&,
            const std::string&) override;

    virtual Botan::Private_Key* private_key_for(
            const Botan::X509_Certificate& cert,
            const std::string&,
            const std::string&) override;
};

class secure_id : public node_id, public uses_botan
{
    friend class secure_id_credentials_manager;
public:
    secure_id();

    secure_id(const secure_id&) = default;
    secure_id(secure_id&&) = default;
    secure_id(const std::vector<uint8_t>&);
    secure_id(const address&);

    secure_id& operator=(const secure_id&) = default;
    secure_id& operator=(secure_id&&) = default;

    Botan::Credentials_Manager& credentials_manager() { return _credentials_manager; }

private:
    Botan::AutoSeeded_RNG _rng;
    std::unique_ptr<Botan::ECDSA_PrivateKey> _key;
    Botan::X509_Certificate _cert;
    secure_id_credentials_manager _credentials_manager;

    std::unique_ptr<Botan::ECDSA_PrivateKey> generate_key();
    Botan::X509_Certificate generate_x509_certificate(const Botan::Private_Key&);
    static std::string fingerprint(const Botan::X509_Certificate&);
};

}

#endif /* CLOWDER_SECURE_ID_HPP */
