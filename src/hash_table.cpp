/* vim: et sw=4 sts=4 ts=4 : */
#include <clowder/hash_table.hpp>
#include "compat.hpp"
#include "routing_table.hpp"
#include "hash_table_pvt.hpp"

using std::unique_ptr;
using std::string;
using std::vector;

namespace clowder
{

hash_table::pvt::pvt(address contact,
                     node_id id,
                     string network_id)
    : _contact(std::move(contact)),
      _network_id(std::move(network_id)),
      _id(id),
      _routes(_id)
{

}

hash_table::hash_table(address contact, node_id id, string network_id)
    : _pimpl(make_unique<pvt>(std::move(contact),
                              std::move(id),
                              std::move(network_id)))
{

}

hash_table::~hash_table() = default;

const node_id& hash_table::id() const { return _pimpl->id(); }

}
