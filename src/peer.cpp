/* vim: et sw=4 sts=4 ts=4 : */

#include "peer.hpp"
#include "compat.hpp"

using std::shared_ptr;

namespace clowder
{

peer::peer(node_id id, shared_ptr<address> addr)
    : _id(id), _addr(std::move(addr))
{

}

}
