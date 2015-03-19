/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_BENCODE_HPP
#define CLOWDER_BENCODE_HPP

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
    virtual ~decoder() = default;
    void parse(const std::string& str);
    void parse(std::istream& is);

private:
    void parse_string(std::istream& is);
    void parse_integer(std::istream& is);

protected:
    virtual void integer(uint64_t);
    virtual void byte_string(std::string);

    virtual void begin_list();
    virtual void begin_dictionary();
    virtual void end();
};

}

}

#endif /* CLOWDER_BENCODE_HPP */
