/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <vector>
#include <clowder/address.hpp>
#include "../src/secure_id.hpp"

using std::vector;

SCENARIO("a secure_id is created")
{
    clowder::address addr(vector<uint8_t>{'C', 'B', 'A'}, {'D'});
    GIVEN("a default constructed secure_id") {
        clowder::secure_id id;
        THEN("the node id shouldn't be all zeros") {
            clowder::node_id zeros("0000000000000000000000000000000000000000000000000000000000000000");
            REQUIRE(id.to_string() != zeros.to_string());
        }
    }

    GIVEN("a secure_id constructed from an address") {
        clowder::secure_id id(addr);

        THEN("the first 4 bytes should be 4e09b60a") {
            std::string crc(id.to_string().substr(0, 8));
            REQUIRE(crc == "4E09B60A");
        }
    }
}

