/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_BENCODE_HPP
#define CLOWDER_BENCODE_HPP

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cassert>

namespace clowder
{

namespace bencode
{

class decoder
{
public:
    void parse(const std::string& str)
    {
        std::istringstream ss(str);
        parse(ss);
    }

    void parse(std::istream& is)
    {
        size_t nesting = 0;
        char c;

        if (!is.good()) {
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
        while (nesting > 0 && is.good());
    }

private:
    void parse_string(std::istream& is)
    {
        size_t length;
        is >> length;

        if (is.bad()) {
            throw std::runtime_error("could not parse string length");
        }

        char c;
        is >> c;

        if (is.bad() || c != ':') {
            throw std::runtime_error("invalid string separator");
        }

        is.width(length);

        std::string data;
        is >> data;

        byte_string(data);
    }

    void parse_integer(std::istream& is)
    {
        char c;
        is >> c;

        if (is.bad() || c != 'i') {
            throw std::runtime_error("invalid integer");
        }

        uint64_t data;
        is >> data;

        if (is.bad()) {
            throw std::runtime_error("unable to parse integer");
        }

        is >> c;
        if (c != 'e') {
            throw std::runtime_error("bad integer terminator");
        }

        integer(data);
    }

protected:
    virtual void integer(uint64_t) {}
    virtual void byte_string(std::string) {}

    virtual void begin_list() {}
    virtual void begin_dictionary() {}
    virtual void end() {}
};

}

}

#endif /* CLOWDER_BENCODE_HPP */
