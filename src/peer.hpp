/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_PEER_HPP
#define CLOWDER_PEER_HPP

#include <clowder/node_id.hpp>
#include <clowder/address.hpp>

#include <memory>
#include <vector>

namespace clowder
{

class peer
{
private:
    node_id _id;
    address _addr;

public:
    peer(node_id id, address addr);

    peer(const peer&) = default;
    peer(peer&&) = default;

    peer& operator=(peer&&) = default;
    peer& operator=(const peer&) = default;

    const node_id& id() const { return _id; }
    const std::vector<uint8_t>& addr();
};

}

#endif /* CLOWDER_PEER_HPP */
