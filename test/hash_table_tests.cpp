/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include "hash_table_tests.hpp"

#include <vector>

using std::vector;

#include <clowder/hash_table.hpp>
#include <clowder/address.hpp>

SCENARIO("create a hash table", "[hash_table]")
{
    std::string netid("ID");
    clowder::address addr(vector<uint8_t>{},{});
    test_hash_table table(std::move(addr), netid);
}
