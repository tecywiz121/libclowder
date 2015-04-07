/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include "hash_table_tests.hpp"

#include "../src/channel_pvt.hpp"

#include <clowder/hash_table.hpp>
#include <clowder/channel.hpp>

#include <vector>

using std::vector;

class test_channel : public clowder::channel
{
public:
    test_channel(test_hash_table& t, clowder::address a)
        : channel(t, a)
    {

    }

    clowder::channel::pvt& pimpl() { return *_pimpl; }

protected:
    virtual void outgoing(const uint8_t*, size_t) override {}
};

SCENARIO("channels each have an address")
{
    clowder::address addr(vector<uint8_t>{},{});
    test_hash_table table(addr, "MYID");

    WHEN("a channel is created") {
        test_channel tc(table, clowder::address{vector<uint8_t>{22, 33}, {44}});
        THEN("the channel's address should match") {
            clowder::address expected{vector<uint8_t>{22, 33}, {44}};
            REQUIRE(tc.addr() == expected);
        }
    }
}

SCENARIO("data can be delivered to/received by a channel")
{
    clowder::address addr({},{});
    test_hash_table table(addr, "MYID");

    test_channel tc(table, clowder::address{{},{}});

    WHEN("empty data is delivered to a channel") {
        uint8_t data[] = {0};
        tc.incoming(data, 0);
    }

    WHEN("empty data is sent from a channel") {
        uint8_t data[] = {0};
        tc.pimpl().send(data, 0);
    }
}
