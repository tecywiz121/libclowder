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

hash_table::pvt::pvt(hash_table& parent, address contact, string network_id)
    : _parent(parent),
      _contact(std::move(contact)),
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

channel& hash_table::pvt::get_channel(const address& addr)
{
    try {
        return *_channels.at(addr);
    } catch (std::out_of_range&) {
        auto result =
                _channels.emplace(std::make_pair(addr,
                                                 _parent.create_channel(addr)));
        return *result.first->second;
    }
}

hash_table::hash_table(address contact, string network_id)
    : _pimpl(make_unique<pvt>(*this, std::move(contact), std::move(network_id)))
{

}

hash_table::~hash_table() = default;

const node_id& hash_table::id() const { return _pimpl->id(); }

channel& hash_table::get_channel(const address& addr)
{
    return _pimpl->get_channel(addr);
}

}
