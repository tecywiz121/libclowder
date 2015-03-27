/* vim: et sw=4 sts=4 ts=4 : */
#include <clowder/channel.hpp>
#include <clowder/hash_table.hpp>
#include "compat.hpp"

#include "hash_table_pvt.hpp"

namespace clowder
{

class channel::pvt
{
private:
    channel& _parent;
    hash_table::pvt& _table;
    address _addr;

public:
    pvt(channel& parent, hash_table::pvt& table, address addr)
        : _parent(parent), _table(table), _addr(std::move(addr))
    {
        // this pvt instance hasn't been created yet, so can't use channel.addr()
        _table.add_channel(_addr, _parent);
    }

    ~pvt()
    {
        _table.remove_channel(_parent);
    }

    const address& addr() const { return _addr; }
};

channel::channel(hash_table& table, address addr)
    : _pimpl(make_unique<pvt>(*this, *table._pimpl, std::move(addr)))
{
}

channel::~channel() = default;

const address& channel::addr() const
{
    return _pimpl->addr();
}

void channel::outgoing(const std::vector<uint8_t>& data)
{
    outgoing(data.data(), data.size());
}

}
