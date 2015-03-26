/* vim: et sw=4 sts=4 ts=4 : */

#include "bencode.hpp"

#include <sstream>

using std::string;
using std::istream;
using std::ostream;

namespace clowder
{

namespace bencode
{

//
// Decoder
//
void decoder::parse(const string& str)
{
    std::istringstream ss(str);
    parse(ss);
}

void decoder::parse(istream& is)
{
    size_t nesting = 0;
    bool parsing = true;

    if (!is) {
        return;
    }

    do {
        char c = is.peek();
        switch (c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                parse_string(is);
                break;

            case 'i':
                parse_integer(is);
                break;

            case 'l':
                nesting++;
                is >> c;
                begin_list();
                break;

            case 'd':
                nesting++;
                is >> c;
                begin_dictionary();
                break;

            case 'e':
                is >> c;
                if (nesting > 0) {
                    nesting--;
                    end();
                } else {
                    throw std::runtime_error("extra end");
                }
                break;
            default:
                // Unrecognized character? Bail out.
                parsing = false;
                break;
        }
    } while (parsing && nesting > 0 && is);

    if (0 < nesting) {
        throw std::runtime_error("unterminated list or dictionary");
    }
}

void decoder::parse_string(istream& is)
{
    size_t length;
    is >> length;

    if (!is) {
        throw std::runtime_error("could not parse string length");
    }

    char c;
    is >> c;

    if (!is || c != ':') {
        throw std::runtime_error("invalid string separator");
    }

    std::string data;

    if (0 < length) {
        is.width(length);
        is >> data;
    }

    byte_string(data);
}

void decoder::parse_integer(istream& is)
{
    char c;
    is >> c;

    // unless something is broken with the istream, we know c must be 'i'
    // because we peeked in parse.

    uint64_t data;
    is >> data;

    if (!is) {
        throw std::runtime_error("unable to parse integer");
    }

    is >> c;
    if (c != 'e') {
        throw std::runtime_error("bad integer terminator");
    }

    integer(data);
}

void decoder::integer(uint64_t) {}
void decoder::byte_string(string) {}
void decoder::begin_list() {}
void decoder::begin_dictionary() {}
void decoder::end() {}

//
// Encoder
//
encoder::encoder(ostream& os)
    : _stream(os)
{

}

void encoder::integer(uint64_t v)
{
    check_state();
    _stream << "i" << v << "e";
}

void encoder::byte_string(string v)
{
    // Don't call check_state() here, byte_string is allowed to be a dict key.

    if (!_state.empty()) {
        state& current = _state.top();
        if (current.expects_key) {
            // Flip the expecting key state to allow all types as dict values.
            current.expects_key = false;

            // Guarantee the ordering of dictionary keys
            if (current.collection == type::dictionary) {
                if (current.lastkey > v) {
                    throw std::runtime_error("dictionary keys must be in lexicographical order");
                } else {
                    current.lastkey = v;
                }
            }
        } else {
            current.expects_key = true;
        }
    }

    _stream << v.size() << ":" << v;
}

void encoder::begin_list()
{
    check_state();
    _state.emplace(type::list);

    _stream << "l";
}

void encoder::begin_dictionary()
{
    check_state();
    _state.emplace(type::dictionary, true);

    _stream << "d";
}

void encoder::check_state()
{
    if (_state.empty()) {
        return;
    }

    state& current = _state.top();

    if (current.collection == type::dictionary) {
        if (current.expects_key) {
            throw std::runtime_error("only byte_string is allowed as dictionary key");
        } else {
            current.expects_key = true;
        }
    }
}

void encoder::end()
{
    if (_state.empty()) {
        throw std::runtime_error("nothing to end");
    }

    state& current = _state.top();
    if (current.collection == type::dictionary) {
        if (!current.expects_key) {
            throw std::runtime_error("dictionary missing value");
        }
    }

    _state.pop();
    _stream << "e";
}

}

}
