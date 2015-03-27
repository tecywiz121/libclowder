/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_HASH_TABLE_PVT_HPP
#define CLOWDER_HASH_TABLE_PVT_HPP

#include <clowder/hash_table.hpp>
#include <clowder/address.hpp>
#include "secure_id.hpp"
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
    secure_id _id;
    routing_table _routes;

    typedef std::unordered_map<address, std::reference_wrapper<channel>> channels_type;
    channels_type _channels;

public:
    pvt(address contact, std::string network_id);
    const node_id& id() const;

    void add_channel(address addr, channel& c);
    void remove_channel(const channel& c);
};

}

#endif /* CLOWDER_HASH_TABLE_PVT_HPP */
