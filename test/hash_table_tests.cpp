/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <clowder/hash_table.hpp>
#include <clowder/address.hpp>

class hash_table_address : public clowder::address
{
public:
    virtual std::vector<uint8_t> to_bytes() const override
    {
        std::vector<uint8_t> value;
        value.emplace_back(12);
        return value;
    }
};

SCENARIO("create a hash table", "[hash_table]")
{
    std::unique_ptr<hash_table_address> addr(new hash_table_address());
    std::string netid("ID");
    clowder::hash_table table(std::move(addr), netid);
}
