/* vim: et sw=4 sts=4 ts=4 : */

#include "peer.hpp"
#include "compat.hpp"

using std::vector;

namespace clowder
{

peer::peer(node_id id, address addr)
    : _id(id), _addr(std::move(addr))
{

}

}
