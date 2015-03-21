/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_BENCODE_HPP
#define CLOWDER_BENCODE_HPP

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <stack>

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

class encoder final
{
public:
    encoder(const encoder&) = default;
    encoder(encoder&&) = default;
    ~encoder() = default;

    explicit encoder(std::ostream&);

    void integer(uint64_t);
    void byte_string(std::string);

    void begin_list();
    void begin_dictionary();
    void end();

private:
    enum class type
    {
        list,
        dictionary,
    };

    struct state
    {
        type collection;
        std::string lastkey;
        bool expects_key;

        state(type c, bool ek = false, std::string lk = std::string())
            : collection(c), lastkey(lk), expects_key(ek) { }
    };

    std::stack<state> _state;
    std::ostream& _stream;

    void check_state();
};

}

}

#endif /* CLOWDER_BENCODE_HPP */
