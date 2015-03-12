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

class test_hash_table : public clowder::hash_table
{
using clowder::hash_table::hash_table;
protected:
    virtual std::unique_ptr<clowder::address> address_from(const uint8_t* data,
                                                           size_t len) override
    {
        REQUIRE(len == 1);
        REQUIRE(data[0] == 12);
        return std::unique_ptr<clowder::address>(new hash_table_address());
    }

    virtual void output(const clowder::address&,
                        const uint8_t*,
                        size_t) override
    {
        // TODO - SW: Verify what is output
    }
};

SCENARIO("create a hash table", "[hash_table]")
{
    std::unique_ptr<hash_table_address> addr(new hash_table_address());
    std::string netid("ID");
    test_hash_table table(std::move(addr), netid);
}
