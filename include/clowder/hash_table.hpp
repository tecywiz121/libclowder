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

    virtual ~hash_table();

    const node_id& id() const;
    channel& get_channel(const address&);

protected:
    virtual std::unique_ptr<channel> create_channel(address) const =0;
};

}

#endif /* CLOWDER_HASH_TABLE_HPP */
