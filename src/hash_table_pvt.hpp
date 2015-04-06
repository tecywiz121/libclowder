/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_HASH_TABLE_PVT_HPP
#define CLOWDER_HASH_TABLE_PVT_HPP

#include <clowder/hash_table.hpp>
#include <clowder/address.hpp>
#include "secure_id.hpp"
#include "routing_table.hpp"
#include "state.hpp"

#include <botan/tls_session_manager.h>
#include <botan/tls_policy.h>

#include <string>
#include <functional>
#include <unordered_map>

namespace clowder
{

class hash_table::pvt : public uses_botan
{
private:
    address _contact;
    std::string _network_id;
    secure_id _id;
    routing_table _routes;
    Botan::AutoSeeded_RNG _rng;
    Botan::TLS::Session_Manager_In_Memory _session_manager;
    Botan::TLS::Strict_Policy _policy;

    typedef std::unordered_map<address, std::reference_wrapper<channel>> channels_type;
    channels_type _channels;

public:
    pvt(address contact, std::string network_id);
    const node_id& id() const;
    secure_id& id() { return _id; }

    void add_channel(address addr, channel& c);
    void remove_channel(const channel& c);

    Botan::TLS::Session_Manager& session_manager() { return _session_manager; }
    const Botan::TLS::Session_Manager& session_manager() const { return _session_manager; }

    Botan::TLS::Policy& policy() { return _policy; }
    const Botan::TLS::Policy& policy() const { return _policy; }
};

}

#endif /* CLOWDER_HASH_TABLE_PVT_HPP */
