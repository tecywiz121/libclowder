/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_CHANNEL_HPP
#define CLOWDER_CHANNEL_HPP

#include <clowder/common.hpp>
#include <clowder/address.hpp>

#include <memory>
#include <vector>

namespace clowder
{

class hash_table;

class CLOWDER_API channel
{
private:
    class pvt;
    std::unique_ptr<pvt> _pimpl;

public:
    virtual ~channel();

    void incoming(const std::vector<uint8_t>& data);
    void incoming(const uint8_t* data, size_t length);

    const address& addr() const;

protected:
    channel(hash_table&, address);
    virtual void outgoing(const std::vector<uint8_t>& data);
    virtual void outgoing(const uint8_t* data, size_t length) =0;
};

}

#endif /* CLOWDER_CHANNEL_HPP */
