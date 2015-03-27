/* vim: et sw=4 sts=4 ts=4 : */
#include <clowder/address.hpp>

using std::vector;

namespace clowder
{

address::address(vector<uint8_t> device, vector<uint8_t> endpoint)
    : _device(std::move(device)), _endpoint(std::move(endpoint))
{

}

const vector<uint8_t>& address::device() const
{
    return _device;
}

const vector<uint8_t>& address::endpoint() const
{
    return _endpoint;
}

bool address::operator<(const address& o) const
{
    return _device < o._device
           || (_device == o._device && _endpoint < o._endpoint);
}

bool address::operator==(const address& o) const
{
    return _device == o._device && _endpoint == o._endpoint;
}

bool address::operator!=(const address& o) const
{
    return _device != o._device || _endpoint != o._endpoint;
}

}
