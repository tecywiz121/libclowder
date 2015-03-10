/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <clowder/address.hpp>

class test_address : public clowder::address
{
public:
    virtual std::vector<char> to_bytes() const override
    {
        std::vector<char> value;
        value.emplace_back(12);
        return value;
    }
};

SCENARIO("addresses can be converted to bytes")
{
    GIVEN("a simple address") {
        test_address addr;

        WHEN("converted to bytes") {
            std::vector<char> bytes(addr.to_bytes());

            THEN("there should be matching bytes in the vector") {
                REQUIRE(bytes.size() == 1);
                CHECK(bytes[0] == 12);
            }
        }

    }
}