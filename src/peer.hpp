/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_PEER_HPP
#define CLOWDER_PEER_HPP

#include <clowder/node_id.hpp>
#include <clowder/address.hpp>

#include <memory>

namespace clowder
{

class peer
{
private:
    node_id _id;
    std::shared_ptr<address> _addr;

public:
    peer(node_id id, std::shared_ptr<address> addr);

    peer(const peer&) = default;
    peer(peer&&) = default;

    peer& operator=(peer&&) = default;
    peer& operator=(const peer&) = default;

    const node_id& id() const { return _id; }
    const address& addr() const { return *_addr; }
};

}

#endif /* CLOWDER_PEER_HPP */
