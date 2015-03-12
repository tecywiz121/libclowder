/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_HASH_TABLE_HPP
#define CLOWDER_HASH_TABLE_HPP

#include <clowder/node_id.hpp>
#include <clowder/common.hpp>
#include <clowder/address.hpp>

#include <memory>
#include <string>

namespace clowder
{

class CLOWDER_API hash_table
{
private:
    class pvt;
    std::unique_ptr<pvt> _pimpl;

public:
    hash_table(std::unique_ptr<address> contact, std::string network_id);
    hash_table(const hash_table&) = delete;
    hash_table(hash_table&&) = default;

    hash_table& operator=(const hash_table&) = delete;
    hash_table& operator=(hash_table&&) = default;

    const node_id& id() const;

    virtual ~hash_table();

    void available(const address& src, const uint8_t[], size_t);

protected:
    virtual std::unique_ptr<address> address_from(const uint8_t[], size_t) =0;
    virtual std::unique_ptr<address> address_from(const address&);
    virtual void output(const address& addr, const uint8_t[], size_t) =0;
};

}

#endif /* CLOWDER_HASH_TABLE_HPP */
