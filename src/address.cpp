/* vim: et sw=4 sts=4 ts=4 : */

#include <clowder/address.hpp>

namespace clowder
{

address::address() = default;

std::vector<uint8_t> address::to_bytes() const
{
    std::vector<uint8_t> ep(endpoint());
    std::vector<uint8_t> bytes(device());
    bytes.reserve(bytes.size() + ep.size());
    bytes.insert(bytes.end(), ep.begin(), ep.end());
    return bytes;
}

bool address::operator==(const address& other) const
{
    return device() == other.device() && endpoint() == other.endpoint();
}

bool address::operator!=(const address& other) const
{
    return device() != other.device() || endpoint() != other.endpoint();
}

bool address::operator<(const address& other) const
{
    return device() < other.device()
           || (device() == other.device() && endpoint() < other.endpoint());
}

}
