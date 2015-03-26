/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include "../src/secure_id.hpp"

class sec_id_address : public clowder::address
{
public:
    virtual std::vector<uint8_t> device() const override
    {
        std::vector<uint8_t> value;
        value.emplace_back('C');
        value.emplace_back('B');
        value.emplace_back('A');
        return value;
    }

    virtual std::vector<uint8_t> endpoint() const override
    {
        std::vector<uint8_t> value{'D'};
        return value;
    }
};

SCENARIO("a secure_id is created")
{
    GIVEN("a default constructed secure_id") {
        clowder::secure_id id;
        THEN("the node id shouldn't be all zeros") {
            clowder::node_id zeros("0000000000000000000000000000000000000000000000000000000000000000");
            REQUIRE(id.to_string() != zeros.to_string());
        }
    }

    GIVEN("a secure_id constructed from an address") {
        sec_id_address addr;
        clowder::secure_id id(addr);

        THEN("the first 4 bytes should be 4e09b60a") {
            std::string crc(id.to_string().substr(0, 8));
            REQUIRE(crc == "4E09B60A");
        }
    }
}

