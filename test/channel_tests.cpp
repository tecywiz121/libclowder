/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <clowder/hash_table.hpp>
#include <clowder/channel.hpp>

#include <vector>

using std::vector;

class test_channel : public clowder::channel
{
public:
    test_channel(clowder::hash_table& t, clowder::address a)
        : channel(t, a)
    {

    }

protected:
    virtual void outgoing(const uint8_t*, size_t) override {}
};

SCENARIO("channels each have an address")
{
    clowder::address addr(vector<uint8_t>{},{});
    clowder::hash_table table(addr, "MYID");

    WHEN("a channel is created") {
        test_channel tc(table, clowder::address{vector<uint8_t>{22, 33}, {44}});
        THEN("the channel's address should match") {
            clowder::address expected{vector<uint8_t>{22, 33}, {44}};
            REQUIRE(tc.addr() == expected);
        }
    }
}
