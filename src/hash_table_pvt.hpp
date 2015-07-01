/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_HASH_TABLE_PVT_HPP
#define CLOWDER_HASH_TABLE_PVT_HPP

#include <clowder/hash_table.hpp>
#include <clowder/address.hpp>
#include "routing_table.hpp"

#include <string>
#include <functional>
#include <unordered_map>

namespace clowder
{

class hash_table::pvt
{
private:
    address _contact;
    std::string _network_id;
    node_id _id;
    routing_table _routes;

public:
    pvt(address contact,
        node_id id,
        std::string network_id);
    const node_id& id() const { return _id; }
    node_id& id() { return _id; }
};

}

#endif /* CLOWDER_HASH_TABLE_PVT_HPP */
