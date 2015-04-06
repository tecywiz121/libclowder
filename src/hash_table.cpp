/* vim: et sw=4 sts=4 ts=4 : */
#include <clowder/hash_table.hpp>
#include <clowder/channel.hpp>
#include "compat.hpp"
#include "secure_id.hpp"
#include "routing_table.hpp"
#include "hash_table_pvt.hpp"

using std::unique_ptr;
using std::string;
using std::vector;

namespace clowder
{

hash_table::pvt::pvt(address contact, string network_id)
    : _contact(std::move(contact)),
      _network_id(std::move(network_id)),
      _id(_contact),
      _routes(_id),
      _session_manager(_rng)
{

}

const node_id& hash_table::pvt::id() const
{
    return _id;
}

void hash_table::pvt::add_channel(address addr, channel& c)
{
    _channels.emplace(addr, std::ref(c));
}

void hash_table::pvt::remove_channel(const channel& c)
{
    _channels.erase(c.addr());
}

hash_table::hash_table(address contact, string network_id)
    : _pimpl(make_unique<pvt>(std::move(contact), std::move(network_id)))
{

}

hash_table::~hash_table() = default;

const node_id& hash_table::id() const { return _pimpl->id(); }

}
