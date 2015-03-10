/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_ROUTING_TABLE_HPP
#define CLOWDER_ROUTING_TABLE_HPP

#include <clowder/node_id.hpp>

#include "compat.hpp"
#include "peer.hpp"

#include <array>
#include <vector>

namespace clowder
{

class routing_table
{
    friend class routing_table_pvt;
public:
    static constexpr size_t n_buckets = node_id::length * 8u;
    static constexpr size_t bucket_size = 20u;

    explicit routing_table(node_id&& self);
    explicit routing_table(const node_id& self);

    void update(const peer& p);
    std::vector<peer> closest(const node_id& target, size_t n) const;

private:
    typedef std::vector<peer> bucket_type;
    const node_id _self;
    std::array<bucket_type, n_buckets> _buckets;
};

}

#endif /* CLOWDER_ROUTING_TABLE_HPP */
