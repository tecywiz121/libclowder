/* vim: et sw=4 sts=4 ts=4 : */
#include <clowder/hash_table.hpp>
#include "compat.hpp"
#include "secure_id.hpp"
#include "routing_table.hpp"

using std::unique_ptr;
using std::string;

namespace clowder
{

class hash_table::pvt
{
private:
    unique_ptr<address> _contact;
    string _network_id;
    secure_id _id;
    routing_table _routes;

public:
    pvt(unique_ptr<address> contact, string network_id)
        : _contact(std::move(contact)),
          _network_id(std::move(network_id)),
          _id(*_contact),
          _routes(_id)
    {

    }

};

hash_table::hash_table(unique_ptr<address> contact, string network_id)
    : _pimpl(make_unique<pvt>(std::move(contact), std::move(network_id)))
{

}

hash_table::~hash_table() = default;

}
