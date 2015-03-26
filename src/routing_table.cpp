/* vim: et sw=4 sts=4 ts=4 : */
#include "routing_table.hpp"

#include <algorithm>
#include <iostream>

namespace clowder
{

routing_table::routing_table(node_id&& self)
    : _self(std::move(self))
{

}

routing_table::routing_table(const node_id& self)
    : _self(self)
{

}

void routing_table::update(const peer& p)
{
    node_id::size_type zeros = (_self ^ p.id()).leading_zeros();

    if (zeros >= n_buckets) {
        return;     // Can't update our own node id
    }

    bucket_type& bucket = _buckets[zeros];

    bucket_type::iterator elem_iter = std::find_if(std::begin(bucket),
                                                   std::end(bucket),
                                                   [&p] (const peer& o) {
                                                       return p.id() == o.id();
                                                   });

    if (std::end(bucket) == elem_iter) {
        if (bucket.size() >= bucket_size) {
            // TODO - SW: Evict oldest node if it doesn't respond to ping.
            throw not_implemented("evict oldest node if it doesn't respond to "
                                  "a ping");
        } else {
            bucket.push_back(p);
        }
    } else {
        std::rotate(elem_iter, elem_iter+1, std::end(bucket));
    }
}

std::vector<peer> routing_table::closest(const node_id& target, size_t n) const
{
    node_id::size_type zeros = (_self ^ target).leading_zeros();
    node_id::size_type distance = 0;

    // TODO - SW: Check that this is the correct behaviour
    if (256 == zeros) {
        zeros = 255;
    }

    std::vector<peer> peers;
    peers.reserve(n);

    std::array<const bucket_type*, 2> buckets;

    // Stop iterating when we have enough peers, or when we're out of buckets
    while (peers.size() < n) {
        size_t count = 0;

        if (zeros >= distance && distance > 0) {
            buckets[count++] = &_buckets[zeros - distance];
        }

        if (zeros + distance < n_buckets) {
            buckets[count++] = &_buckets[zeros + distance];
        }

        if (0 == count) {
            break;  // No more buckets to check
        }

        for (size_t ii = 0; ii < count; ii++) {
            const bucket_type& bucket = *buckets[ii];

            for (size_t jj = 0; jj < bucket.size(); jj++) {
                peers.emplace_back(bucket[jj]);
            }
        }

        distance++;
    }

    std::sort(peers.begin(),
              peers.end(),
              [&target] (const peer& lhs, const peer& rhs) {
                return (target^lhs.id()) < (target^rhs.id());
              });

    if (peers.size() > n) {
        peers.erase(peers.begin()+n, peers.end());
    }

    return peers;
}

}
