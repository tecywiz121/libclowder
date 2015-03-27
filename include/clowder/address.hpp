/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_ADDRESS_HPP
#define CLOWDER_ADDRESS_HPP

#include <clowder/common.hpp>

#include <vector>
#include <functional>

namespace clowder
{

class CLOWDER_API address
{
private:
    std::vector<uint8_t> _device;
    std::vector<uint8_t> _endpoint;

public:
    address(std::vector<uint8_t>, std::vector<uint8_t>);

    const std::vector<uint8_t>& device() const;     // Equivalent to IP address
    const std::vector<uint8_t>& endpoint() const;   // Equivalent to port

    bool operator<(const address& o) const;
    bool operator==(const address& o) const;
    bool operator!=(const address& o) const;
};

}

namespace std
{

#pragma GCC diagnostic push
/*
 * Disable clang's mismatched tags warning here because in some versions of
 * libstdc++ hash is defined as a class instead of as a struct. In practice
 * this distinction is meaningless, but clang warns about it, so...
 */

#if __clang__
#   pragma clang diagnostic ignored "-Wmismatched-tags"
#endif
template<>
struct CLOWDER_API hash<clowder::address>
{
    typedef clowder::address argument_type;
    typedef std::size_t result_type;

    result_type operator()(argument_type const& s) const
    {
        result_type h = 0;
        for (const uint8_t vv : s.device()) {
            hash_combine(h, std::hash<uint8_t>()(vv));
        }
        for (const uint8_t vv : s.endpoint()) {
            hash_combine(h, std::hash<uint8_t>()(vv));
        }
        return h;
    }
private:
    static void hash_combine(result_type& h, const result_type& v)
    {
        // Adapted from boost::hash_combine
        h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
};
#pragma GCC diagnostic pop

}

#endif /* CLOWDER_ADDRESS_HPP */
