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

#if 0
namespace bencode
{

class object;
class object_collection;

class child_vector
{
    typedef std::vector<std::unique_ptr<object>> data_type;

public:
    class iterator
    {
        friend class clowder::bencode::object_collection;
    private:
        data_type::iterator _i;

        explicit iterator(data_type::iterator i)
            : _i(i) {}

    public:
        iterator() = default;
        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;
        iterator& operator=(iterator&&) = default;
        ~iterator() = default;

        object& operator*() { return **_i; }
        const object& operator*() const { return **_i; }

        object* operator->() const { return (*_i).get(); }

        iterator& operator++() { ++_i; return *this; }
        iterator operator++(int) { iterator t = *this; ++_i; return t; }

        iterator& operator+=(data_type::size_type n) { _i += n; return *this; }
        iterator& operator-=(data_type::size_type n) { _i -= n; return *this; }

        iterator operator+(data_type::size_type n) { return iterator(_i + n); }
        iterator operator-(data_type::size_type n) { return iterator(_i - n); }

        data_type::difference_type operator-(const iterator& o) { return _i - o._i; }

        object& operator[](data_type::size_type n) { return *_i[n]; }
        const object& operator[](data_type::size_type n) const { return *_i[n]; }

        bool operator>(const iterator& o) const { return _i > o._i; }
        bool operator>=(const iterator& o) const { return _i >= o._i; }
        bool operator<(const iterator& o) const { return _i < o._i; }
        bool operator<=(const iterator& o) const { return _i <= o._i; }

        iterator& operator--() { --_i; return *this; }
        iterator operator--(int) { iterator t = *this; --_i; return t; }

        bool operator==(const iterator& o) const { return _i == o._i; }
        bool operator!=(const iterator& o) const { return _i != o._i; }
    };

    typedef object& value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type reference;
    typedef const value_type const_reference;
    typedef const iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    child_vector(data_type&);

    reference at(size_type pos);
    const_reference at(size_type pos) const;

    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;

    reference front();
    const_reference front() const;

    reference back();
    const_reference back() const;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    bool empty() const;
    size_type size() const;

private:
    data_type& _data;
};

class object
{
    friend std::istream& operator>>(std::istream& is, object& obj)
    {
        obj.read_from(is);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const object& obj)
    {
        obj.write_to(os);
        return os;
    }

public:
    virtual ~object() = default;

protected:
    virtual void write_to(std::ostream& os) const =0;
    virtual void read_from(std::istream& is) =0;

    static std::unique_ptr<object> create(char c);
};

class object_collection : public object
{
public:
    child_vector children() { return child_vector(_children); }

    child_vector::iterator erase(child_vector::const_iterator pos)
    {
        return child_vector::iterator(_children.erase(pos._i));
    }

    child_vector::iterator erase(child_vector::const_iterator first,
                                 child_vector::const_iterator last)
    {
        return child_vector::iterator(_children.erase(first._i, last._i));
    }


protected:
    std::vector<std::unique_ptr<object>> _children;
};

class integer : public object
{
private:
    uint64_t _value;

public:
    integer() = default;
    explicit integer(uint64_t v) : _value(v) { }
    uint64_t value() const { return _value; }
    void value(uint64_t v) { _value = v; }

protected:
    virtual void write_to(std::ostream& os) const override
    {
        os << "i" << _value << "e";
    }

    virtual void read_from(std::istream& is) override
    {
        char c;
        is >> c;

        if ('i' != c || is.bad()) {
            throw std::runtime_error("not an integer");
        }

        is >> _value;

        if (is.bad()) {
            throw std::runtime_error("bad integer");
        }

        is >> c;

        if ('e' != c || is.bad()) {
            throw std::runtime_error("no terminator");
        }
    }
};

class byte_string : public object
{
private:
    std::string _value;

public:
    byte_string() = default;
    byte_string(const byte_string&) = default;
    byte_string(byte_string&&) = default;

    explicit byte_string(std::string s) : _value(s) { }
    const std::string& value() const { return _value; }
    void value(std::string s) { _value = s; }

protected:
    virtual void write_to(std::ostream& os) const override
    {
        os << _value.size() << ":" << _value;
    }

    virtual void read_from(std::istream& is) override
    {
        size_t length;
        is >> length;

        if (is.bad()) {
            throw std::runtime_error("length not a number");
        }

        char c;
        is >> c;

        if (':' != c) {
            throw std::runtime_error("bad string separator");
        }

        is.width(length);
        is >> _value;
    }
};

class list : public object_collection
{
public:
    template<typename T, typename... TArgs>
    void push_back(TArgs&& ... args)
    {
        _children.push_back(make_unique<T>(std::forward<TArgs>(args)...));
    }

protected:
    virtual void write_to(std::ostream& os) const override
    {
        os << "l";

        for (const unique_ptr<object>& o : _children) {
            os << *o;
        }

        os << "e";
    }

    virtual void read_from(std::istream& is) override
    {
        char c;
        is >> c;
        if (is.bad() || c != 'l') {
            throw std::runtime_error("not a list");
        }

        while (is.good() && 'e' != (c = is.peek())) {
            std::unique_ptr<object> child = object::create(c);
            is >> *child;
            _children.push_back(child);
        }

        if (is.bad() || c != 'e') {
            throw std::runtime_error("invalid list terminator");
        }
    }
};

class pair : public object
{
    friend class dictionary;
private:
    byte_string _key;
    std::unique_ptr<object> _value;

public:
    const std::string& key() const { return _key.value(); }
    void key(std::string v) { _key.value(v); }

    template<typename T, typename... TArgs>
    T& value(TArgs&& ...args)
    {
        return *(_value = make_unique<T>(std::forward<TArgs>(args)...));
    }

    bool has_value() { return _value != nullptr; }
    object& value() { return *_value; }
    const object& value() const { return *_value; }
};

class dictionary : public object_collection
{
protected:
};

std::unique_ptr<object> object::create(char c)
{
    switch (c) {
        case 'i':
            return unique_ptr<object>(dynamic_cast<object*>(new integer()));
        case 'l':
            return unique_ptr<object>(dynamic_cast<object*>(new list()));
        case 'd':
            return unique_ptr<object>(dynamic_cast<object*>(new dictionary()));
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
            return unique_ptr<object>(dynamic_cast<object*>(new byte_string()));
        default:
            throw std::runtime_error("not a known type");
    }
}

}

#endif
}

#endif /* CLOWDER_BENCODE_HPP */
