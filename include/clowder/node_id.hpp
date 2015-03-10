/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_NODE_ID_HPP
#define CLOWDER_NODE_ID_HPP

#include <clowder/common.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace clowder
{

class CLOWDER_API format_error : public std::runtime_error
{
public:
    explicit format_error(const std::string& what_arg)
        : std::runtime_error(what_arg) {}
    explicit format_error(const char* what_arg)
        : std::runtime_error(what_arg) {}
};

class CLOWDER_API node_id
{
public:
    static constexpr size_t length = 32;
    typedef std::array<uint8_t, length> array_type;
    typedef array_type::size_type size_type;

    node_id();
    explicit node_id(const std::string&);

    node_id(const node_id&) = default;
    node_id(node_id&&) = default;

    node_id& operator=(const node_id&) = default;
    node_id& operator=(node_id&&) = default;

    node_id operator^(const node_id&) const;
    node_id& operator^=(const node_id&);

    bool operator==(const node_id&) const;
    bool operator!=(const node_id&) const;

    bool operator>(const node_id&) const;
    bool operator>=(const node_id&) const;

    bool operator<(const node_id&) const;
    bool operator<=(const node_id&) const;

    void set(const std::string&);
    std::string to_string() const;

    size_type leading_zeros() const;
private:
    array_type _array;
};

}

#endif /* CLOWDER_NODE_ID_HPP */

