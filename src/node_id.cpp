/* vim: et sw=4 sts=4 ts=4 : */

#include <clowder/node_id.hpp>

#include <limits>
#include <sstream>
#include <iostream>

using std::string;

namespace clowder
{

node_id::node_id()
{
    _array.fill(0);
}

node_id::node_id(const string& str)
{
    set(str);
}

void node_id::set(const string& str)
{
    if (str.length() != 2*length) {
        throw format_error("incorrect node id length");
    }

    for (size_t ii = 0; ii < length; ii++) {
        uint8_t val = 0;

        for (unsigned int jj = 0; jj < 2; jj++) {
            char in = str[(2*ii)+jj];
            if (in >= 65 && in <= 70) {             // Uppercase
                in -= 55;
            } else if (in >= 97 && in <= 102) {     // Lowercase
                in -= 87;
            } else if (in >= 48 && in <= 57) {      // Numeric
                in -= 48;
            } else {
                throw format_error("invalid character in node id");
            }

            val += in << (4*(1-jj));
        }

        _array[ii] = val;
    }
}

string node_id::to_string() const
{
    string result;
    result.reserve(2*length);

    for (size_t ii = 0; ii < length; ii++) {
        for (unsigned int jj = 0; jj < 2; jj++) {
            uint8_t in = _array[ii] >> (4*(1-jj));
            in &= 0xF;
            if (in <= 9) {
                in += 48;
            } else if (in <= 15) {
                in += 55;
            }
            result += in;
        }
    }

    return result;
}

node_id node_id::operator^(const node_id& right) const
{
    node_id result;
    for (array_type::size_type ii = 0; ii < length; ii++)
    {
        result._array[ii] = _array[ii] ^ right._array[ii];
    }
    return result;
}

node_id& node_id::operator^=(const node_id& right)
{
    for (array_type::size_type ii = 0; ii < length; ii++)
    {
        _array[ii] ^= right._array[ii];
    }

    return *this;
}

node_id::size_type node_id::leading_zeros() const
{
    for (size_type ii = 0; ii < length; ii++) {
        for (size_type jj = 0; jj < 8; jj++) {
            if (((_array[ii] >> (7-jj)) & 0x1) != 0) {
                return ii * 8 + jj;
            }
        }
    }

    return length * 8;
}

#define NODE_ID_COMP_OP(op)                             \
bool node_id::operator op(const node_id& other) const   \
{                                                       \
    return _array op other._array;                      \
}

NODE_ID_COMP_OP(==)
NODE_ID_COMP_OP(!=)
NODE_ID_COMP_OP(>)
NODE_ID_COMP_OP(>=)
NODE_ID_COMP_OP(<)
NODE_ID_COMP_OP(<=)

}
