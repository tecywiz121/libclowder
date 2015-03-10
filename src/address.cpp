/* vim: et sw=4 sts=4 ts=4 : */

#include <clowder/address.hpp>

namespace clowder
{

address::address() = default;

bool address::operator==(const address& other) const
{
    return to_bytes() == other.to_bytes();
}

bool address::operator!=(const address& other) const
{
    return to_bytes() != other.to_bytes();
}

}
