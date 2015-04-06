/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_CHANNEL_PVT_HPP
#define CLOWDER_CHANNEL_PVT_HPP

#include <clowder/channel.hpp>

#include "state.hpp"
#include "hash_table_pvt.hpp"

#include <botan/credentials_manager.h>
#include <botan/tls_channel.h>

#include <memory>
#include <vector>

namespace clowder
{

class client_credentials_manager : public Botan::Credentials_Manager
{
public:
    virtual void verify_certificate_chain(const std::string&,
                                          const std::string&,
                                          const std::vector<Botan::X509_Certificate>&) override
    {
        // TODO - SW: Actually verify the certificate based on the given node id
    }
};

class channel::pvt : public uses_botan
{
private:
    channel& _parent;
    hash_table::pvt& _table;
    address _addr;

    bool _was_active = false;
    std::vector<uint8_t> _queue;

    std::unique_ptr<client_credentials_manager> _client_credentials_manager;
    std::unique_ptr<Botan::TLS::Channel> _channel;
    Botan::AutoSeeded_RNG _rng;

    void data_ready(const uint8_t*, size_t);
    void alert(Botan::TLS::Alert, const uint8_t* , size_t );
    bool handshake(const Botan::TLS::Session&);
    void outgoing(const uint8_t* data, size_t length);
    void connect(bool server);

public:
    pvt(channel& parent, hash_table::pvt& table, address addr);
    ~pvt();

    const address& addr() const { return _addr; }

    bool connected();
    void incoming(const uint8_t* data, size_t length);
    void send(const uint8_t* data, size_t length);
};

}

#endif /* CLOWDER_CHANNEL_PVT_HPP */
