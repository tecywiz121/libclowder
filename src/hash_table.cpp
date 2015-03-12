/* vim: et sw=4 sts=4 ts=4 : */
#include <clowder/hash_table.hpp>
#include "compat.hpp"
#include "secure_id.hpp"
#include "routing_table.hpp"

using std::unique_ptr;
using std::string;

namespace clowder
{

class hash_table::pvt
{
private:
    unique_ptr<address> _contact;
    string _network_id;
    secure_id _id;
    routing_table _routes;

public:
    pvt(unique_ptr<address> contact, string network_id)
        : _contact(std::move(contact)),
          _network_id(std::move(network_id)),
          _id(*_contact),
          _routes(_id)
    {

    }

    const node_id& id() const
    {
        return _id;
    }

    void available(const address&, const uint8_t*, size_t)
    {
        throw not_implemented("implement processing incoming data");
    }
};

hash_table::hash_table(unique_ptr<address> contact, string network_id)
    : _pimpl(make_unique<pvt>(std::move(contact), std::move(network_id)))
{

}

hash_table::~hash_table() = default;

const node_id& hash_table::id() const { return _pimpl->id(); }

void hash_table::available(const address& from, const uint8_t* data, size_t len)
{
    _pimpl->available(from, data, len);
}

std::unique_ptr<address> hash_table::address_from(const address& addr)
{
    std::vector<uint8_t> bytes(addr.to_bytes());
    return address_from(bytes.data(), bytes.size());
}
}
