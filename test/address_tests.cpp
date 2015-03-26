/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <clowder/address.hpp>

class test_address : public clowder::address
{
public:
    virtual std::vector<uint8_t> device() const override
    {
        std::vector<uint8_t> value;
        value.emplace_back(12);
        return value;
    }

    virtual std::vector<uint8_t> endpoint() const override
    {
        std::vector<uint8_t> value;
        value.emplace_back(55);
        return value;
    }
};

SCENARIO("addresses can be converted to bytes")
{
    GIVEN("a simple address") {
        test_address addr;

        WHEN("converted to bytes") {
            std::vector<uint8_t> bytes(addr.to_bytes());

            THEN("there should be matching bytes in the vector") {
                std::vector<uint8_t> result{12, 55};
                CHECK(bytes == result);
            }
        }

    }
}
