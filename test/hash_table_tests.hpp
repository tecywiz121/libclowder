/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_HASH_TABLE_TESTS_HPP
#define CLOWDER_HASH_TABLE_TESTS_HPP

#include <clowder/channel.hpp>
#include <clowder/hash_table.hpp>
#include <clowder/address.hpp>

class test_hash_table : public clowder::hash_table
{
public:
    test_hash_table(clowder::address a, std::string n) : hash_table(a,n) {}

protected:
    std::unique_ptr<clowder::channel> create_channel(clowder::address) const override
    {
        return nullptr;
    }
};

#endif /* CLOWDER_HASH_TABLE_TESTS_HPP */
