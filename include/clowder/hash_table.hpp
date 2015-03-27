/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_HASH_TABLE_HPP
#define CLOWDER_HASH_TABLE_HPP

#include <clowder/node_id.hpp>
#include <clowder/common.hpp>
#include <clowder/address.hpp>

#include <memory>
#include <string>
#include <vector>

namespace clowder
{

class channel;

class CLOWDER_API hash_table
{
    friend class channel;
private:
    class pvt;
    std::unique_ptr<pvt> _pimpl;

public:
    hash_table(address contact, std::string network_id);
    hash_table(const hash_table&) = delete;
    hash_table(hash_table&&) = default;

    hash_table& operator=(const hash_table&) = delete;
    hash_table& operator=(hash_table&&) = default;

    const node_id& id() const;

    virtual ~hash_table();

    channel& channel_for(const std::vector<uint8_t>& address);

protected:
};

}

#endif /* CLOWDER_HASH_TABLE_HPP */
