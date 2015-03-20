/* vim: et sw=4 sts=4 ts=4 : */

#include "bencode.hpp"

#include <sstream>

using std::string;
using std::istream;

namespace clowder
{

namespace bencode
{

void decoder::parse(const string& str)
{
    std::istringstream ss(str);
    parse(ss);
}

void decoder::parse(istream& is)
{
    size_t nesting = 0;
    char c;

    if (!is) {
        return;
    }

    do
    {
        c = is.peek();
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
        }
    }
    while (nesting > 0 && is);
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

    if (!is || c != 'i') {
        throw std::runtime_error("invalid integer");
    }

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

}

}
