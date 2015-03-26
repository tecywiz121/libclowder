/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_ADDRESS_HPP
#define CLOWDER_ADDRESS_HPP

#include <clowder/common.hpp>

#include <vector>

namespace clowder
{

class CLOWDER_API address
{
public:
    address();
    address(const address&) = default;
    address(address&&) = default;

    virtual ~address() = default;

    address& operator=(const address&) = default;
    address& operator=(address&&) = default;

    std::vector<uint8_t> to_bytes() const;
    virtual std::vector<uint8_t> device() const =0;     // Equivalent to IP address
    virtual std::vector<uint8_t> endpoint() const =0;   // Equivalent to port

    virtual bool operator<(const address&) const;
    virtual bool operator==(const address&) const;
    virtual bool operator!=(const address&) const;
};

}

#endif /* CLOWDER_ADDRESS_HPP */
