/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <clowder/address.hpp>

#include <vector>

using std::vector;

SCENARIO("an address can be created from two vectors")
{
    GIVEN("two vectors")
    {
        vector<uint8_t> device{1, 2, 3, 4};
        vector<uint8_t> endpoint{5, 6};

        WHEN("an address is constructed by moving")
        {
            vector<uint8_t> d(device);
            vector<uint8_t> e(endpoint);
            clowder::address addr(std::move(d), std::move(e));

            THEN("the address should be correct")
            {
                CHECK(addr.device() == device);
                CHECK(addr.endpoint() == endpoint);
            }
        }

        WHEN("an address is constructed by copying")
        {
            const vector<uint8_t>& d = device;
            const vector<uint8_t>& e = endpoint;
            clowder::address addr(d, e);

            THEN("the address should be correct")
            {
                CHECK(addr.device() == device);
                CHECK(addr.endpoint() == endpoint);
            }
        }
    }
}

SCENARIO("an address can be compared for equality")
{
    GIVEN("an address") {
        clowder::address a({12, 44}, {13, 45});

        WHEN("compared to itself") {
            THEN("it should be equal") {
                REQUIRE(a == a);
            }
            THEN("it should not be not-equal") {
                REQUIRE_FALSE(a != a);
            }
            THEN("it should not be less than") {
                CHECK_FALSE(a < a);
            }
        }

        WHEN("compared to another equal address") {
            clowder::address b({12, 44}, {13, 45});
            THEN("it should be equal") {
                REQUIRE(a == b);
            }
            THEN("it should not be not-equal") {
                REQUIRE_FALSE(a != b);
            }
            THEN("it should not be less than") {
                CHECK_FALSE(a < b);
                CHECK_FALSE(b < a);
            }
        }

        WHEN("compared to another different address") {
            clowder::address b({2, 4}, {13, 45});
            clowder::address c({12, 44}, {3, 45});
            THEN("it should not be equal") {
                CHECK_FALSE(a == b);
                CHECK_FALSE(a == c);
            }
            THEN("it should be not-equal") {
                CHECK(a != b);
                CHECK(a != c);
            }
            THEN("b/c should be less than a") {
                CHECK(b < a);
                CHECK(c < a);
                CHECK_FALSE(a < b);
                CHECK_FALSE(a < c);
            }
        }
    }
}
