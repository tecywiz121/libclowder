/* vim: et sw=4 sts=4 ts=4 : */
#include <clowder/channel.hpp>
#include <clowder/hash_table.hpp>
#include "compat.hpp"
#include "state.hpp"

#include "hash_table_pvt.hpp"
#include "channel_pvt.hpp"

#include <botan/tls_version.h>
#include <botan/tls_channel.h>
#include <botan/tls_server.h>
#include <botan/tls_client.h>

#include <functional>

using std::unique_ptr;
using namespace std::placeholders;

namespace clowder
{

void channel::pvt::data_ready(const uint8_t*, size_t)
{

}

void channel::pvt::alert(Botan::TLS::Alert, const uint8_t* , size_t )
{

}

bool channel::pvt::handshake(const Botan::TLS::Session&)
{
    _was_active = true;
    _channel->send(_queue.data(), _queue.size());
    _queue.clear();
    return true;
}

void channel::pvt::outgoing(const uint8_t* data, size_t length)
{
    _parent.outgoing(data, length);
}

void channel::pvt::connect(bool server)
{
    if (_channel != nullptr) {
        return;
    }

    _was_active = false;

    auto output_fn = std::bind(&channel::pvt::outgoing, this, _1, _2);
    auto data_cb = std::bind(&channel::pvt::data_ready, this, _1, _2);
    auto alert_cb = std::bind(&channel::pvt::alert, this, _1, _2, _3);
    auto handshake_cb = std::bind(&channel::pvt::handshake, this, _1);
    if (server) {
        _channel = std::move(make_unique<Botan::TLS::Server>(output_fn,
                                                             data_cb,
                                                             alert_cb,
                                                             handshake_cb,
                                                             _table.session_manager(),
                                                             _table.id().credentials_manager(),
                                                             _table.policy(),
                                                             _rng,
                                                             std::vector<std::string>(),
                                                             true));
    } else {
        _client_credentials_manager = make_unique<client_credentials_manager>();
        _channel = make_unique<Botan::TLS::Client>(output_fn,
                                                   data_cb,
                                                   alert_cb,
                                                   handshake_cb,
                                                   _table.session_manager(),
                                                   *_client_credentials_manager,
                                                   _table.policy(),
                                                   _rng,
                                                   Botan::TLS::Server_Information(), // TODO - SW: We could probably provide some info here
                                                   Botan::TLS::Protocol_Version::latest_dtls_version());
    }

}

channel::pvt::pvt(channel& parent, hash_table::pvt& table, address addr)
    : _parent(parent), _table(table), _addr(std::move(addr))
{
}

channel::pvt::~pvt()
{

}

bool channel::pvt::connected()
{
    // TODO - SW: Clear the channel when disconnected
    return nullptr != _channel && _channel->is_active();
}

void channel::pvt::incoming(const uint8_t* data, size_t length)
{
    connect(true);
    _channel->received_data(data, length);
}

void channel::pvt::send(const uint8_t* data, size_t length)
{
    connect(false);

    if (_was_active) {
        _channel->send(data, length);
    } else {
        _queue.reserve(_queue.size() + length);
        std::copy(&data[0], &data[length], std::back_inserter(_queue));
    }
}

channel::channel(hash_table& table, address addr)
    : _pimpl(make_unique<pvt>(*this, *table._pimpl, std::move(addr)))
{
}

channel::~channel() = default;

const address& channel::addr() const
{
    return _pimpl->addr();
}

void channel::incoming(const uint8_t* data, size_t length)
{
    _pimpl->incoming(data, length);
}

}
