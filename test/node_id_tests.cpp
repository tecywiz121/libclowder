#include "catch.hpp"

#include <clowder/node_id.hpp>

SCENARIO("nodes have identifiers that can be set", "[node_id]")
{
    GIVEN("A node_id with the default value")
    {
        clowder::node_id id;

        std::string data;
        data.reserve(2*id.length);

        static_assert(id.length % 2u == 0, "node_id::length must be even");
        for (unsigned int ii = 0; ii < id.length; ii++) {
            if (ii%2==0) {
                data += "AB";
            } else {
                data += "56";
            }
        }

        WHEN("setting node id correctly")
        {
            id.set(data);

            THEN("the string value equals the input")
            {
                REQUIRE(id.to_string() == data);
            }
        }

        WHEN("setting a short node id")
        {
            data.pop_back();

            THEN("a format_error should be thrown")
            {
                REQUIRE_THROWS_AS(id.set(data), clowder::format_error);
            }
        }

        WHEN("setting a long node id")
        {
            data.push_back('A');

            THEN("a format_error should be thrown")
            {
                REQUIRE_THROWS_AS(id.set(data), clowder::format_error);
            }
        }

        WHEN("an invalid character is used")
        {
            data.pop_back();
            data.push_back('x');

            THEN("a format_error should be thrown")
            {
                REQUIRE_THROWS_AS(id.set(data), clowder::format_error);
            }
        }
    }
}

SCENARIO("nodes can be xor'd together", "[node_id]")
{
    GIVEN("two node_ids with opposite simple values") {
        clowder::node_id zeros("0000000000000000000000000000000000000000000000000000000000000000");
        clowder::node_id ones("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");

        clowder::node_id id0("00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF");
        clowder::node_id id1("FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00");

        WHEN("xor'ing with themselves") {
            std::string t0 = (id0 ^ id0).to_string();
            std::string t1 = (id1 ^ id1).to_string();

            THEN("the result should be all zeros") {
                CHECK(t0 == zeros.to_string());
                CHECK(t1 == zeros.to_string());
            }
        }

        WHEN("xor'ing with each other") {
            std::string t0 = (id0 ^ id1).to_string();
            std::string t1 = (id1 ^ id0).to_string();

            THEN("the result should be all ones") {
                CHECK(t0 == ones.to_string());
                CHECK(t1 == ones.to_string());
            }
        }

        WHEN("xor-assigning with themselves") {
            id0 ^= id0;
            id1 ^= id1;

            THEN("the result should be all zeros") {
                CHECK(id0.to_string() == zeros.to_string());
                CHECK(id1.to_string() == zeros.to_string());
            }
        }

        WHEN("xor-assigning with each other") {
            id0 ^= id1;

            THEN("the result should be all ones") {
                CHECK(id0.to_string() == ones.to_string());
            }
        }
    }
}

SCENARIO("nodes can count the number of leading zero bits", "[node_id]")
{
    GIVEN("a node") {
        clowder::node_id id;

        WHEN("set to have no leading zeros") {
            id = clowder::node_id("F000000000000000000000000000000000000000000000000000000000000000");

            THEN("the result should be zero") {
                REQUIRE(id.leading_zeros() == 0);
            }
        }

        WHEN("set to have one leading zero") {
            id = clowder::node_id("7000000000000000000000000000000000000000000000000000000000000000");

            THEN("the result should be one") {
                REQUIRE(id.leading_zeros() == 1);
            }
        }

        WHEN("set to have all but two leading zeros") {
            id = clowder::node_id("0000000000000000000000000000000000000000000000000000000000000002");

            THEN("the result should be 254") {
                REQUIRE(id.leading_zeros() == 254);
            }
        }

        WHEN("set to have all but one leading zero") {
            id = clowder::node_id("0000000000000000000000000000000000000000000000000000000000000001");

            THEN("the result should be 255") {
                REQUIRE(id.leading_zeros() == 255);
            }
        }

        WHEN("set to have all zeros") {
            id = clowder::node_id("0000000000000000000000000000000000000000000000000000000000000000");

            THEN("the result should be 256") {
                REQUIRE(id.leading_zeros() == 256);
            }
        }
    }
}

SCENARIO("nodes can be checked for (in)equality", "[node_id]")
{
    GIVEN("one node") {
        clowder::node_id id;

        WHEN("compared to itself") {
            THEN("it should be equal") {
                CHECK(id == id);
                CHECK_FALSE(id != id);

                CHECK(id >= id);
                CHECK_FALSE(id > id);

                CHECK(id <= id);
                CHECK_FALSE(id < id);
            }
        }
    }

    GIVEN("two distinct but equal nodes") {
        clowder::node_id idA("FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00");
        clowder::node_id idB("FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00");

        WHEN("compared to each other") {
            THEN("they should be equal") {
                CHECK(idA == idB);
                CHECK(idB == idA);

                CHECK_FALSE(idA != idB);
                CHECK_FALSE(idB != idA);

                CHECK(idA >= idB);
                CHECK_FALSE(idA > idB);

                CHECK(idA <= idB);
                CHECK_FALSE(idA < idB);
            }
        }
    }

    GIVEN("two distinct and different nodes") {
        clowder::node_id idA("0F00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00");
        clowder::node_id idB("FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00");

        WHEN("compared to each other") {
            THEN("they should be non-equal") {
                CHECK(idA != idB);
                CHECK(idB != idA);
                CHECK_FALSE(idA == idB);
                CHECK_FALSE(idB == idA);

                CHECK(idA <= idB);
                CHECK(idA < idB);

                CHECK_FALSE(idA >= idB);
                CHECK_FALSE(idA > idB);

                CHECK_FALSE(idB <= idA);
                CHECK_FALSE(idB < idA);

                CHECK(idB >= idA);
                CHECK(idB > idA);
            }
        }
    }
}
