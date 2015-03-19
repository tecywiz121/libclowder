/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <vector>

#include "../src/bencode.hpp"

using clowder::bencode::decoder;

class event
{
public:
    enum class event_kind
    {
        integer,
        byte_string,
        begin_list,
        begin_dictionary,
        end,
    };

    event_kind kind;
    uint64_t integer;
    std::string byte_string;

    ~event() = default;
    event(event_kind k) : kind(k) {}
    event(event_kind k, uint64_t i) : kind(k), integer(i) {}
    event(event_kind k, std::string s) : kind(k), byte_string(s) {}

    bool operator==(const event& o) const
    {
        if (kind != o.kind) {
            return false;
        }

        switch (kind) {
            case event_kind::begin_list:
            case event_kind::begin_dictionary:
            case event_kind::end:
                return true;
            case event_kind::integer:
                return integer == o.integer;
            case event_kind::byte_string:
                return byte_string == o.byte_string;
            default:
                throw std::logic_error("WTF?");
        }
    }

    bool operator!=(const event& o) const
    {
        return !(*this == o);
    }
};

class test_decoder : public decoder
{
public:
    size_t n_events = 0;
    std::vector<event> events;

protected:
    virtual void integer(uint64_t v) override
    {
        n_events++;
        events.emplace_back(event::event_kind::integer, v);
        std::cout << "Integer: " << v << std::endl;
    }

    virtual void byte_string(std::string v) override
    {
        n_events++;
        events.emplace_back(event::event_kind::byte_string, v);
        std::cout << "String: " << v << std::endl;
    }

    virtual void begin_list() override
    {
        n_events++;
        events.emplace_back(event::event_kind::begin_list);
        std::cout << "Begin: list" << std::endl;
    }

    virtual void begin_dictionary() override
    {
        n_events++;
        events.emplace_back(event::event_kind::begin_dictionary);
        std::cout << "Begin: dictionary" << std::endl;
    }

    virtual void end() override
    {
        n_events++;
        events.emplace_back(event::event_kind::end);
        std::cout << "End" << std::endl;
    }
};

SCENARIO("A decoder can parse some data")
{
    test_decoder dec;

    GIVEN("An empty string") {
        std::string data("");

        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("Nothing should happen") {
                REQUIRE(dec.n_events == 0);
            }
        }
    }

    GIVEN("A single integer followed by garbage") {
        std::string data("i10eoauoaeu");
        std::vector<event> expect{event(event::event_kind::integer, 10)};

        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("One integer event should occur") {
                REQUIRE(dec.events == expect);
            }
        }
    }

    GIVEN("A single integer") {
        std::string data("i10e");
        std::vector<event> expect{event(event::event_kind::integer, 10)};

        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("One integer event should occur") {
                REQUIRE(dec.events == expect);
            }
        }
    }

    GIVEN("An empty list") {
        std::string data("le");
        std::vector<event> expect{event(event::event_kind::begin_list),
                                  event(event::event_kind::end)};
        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("One begin_list and one end should occur") {
                REQUIRE(dec.events == expect);
            }
        }
    }

    GIVEN("A list with one integer") {
        std::string data("li4564ee");
        std::vector<event> expect{event(event::event_kind::begin_list),
                                  event(event::event_kind::integer, 4564),
                                  event(event::event_kind::end)};
        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("One begin_list, integer, then an end should occur") {
                REQUIRE(dec.events == expect);
            }
        }
    }

    GIVEN("A dictionary with one key and one value") {
        std::string data("d4:spami18446744073709551615ee");
        std::vector<event> expect{event(event::event_kind::begin_dictionary),
                                  event(event::event_kind::byte_string, "spam"),
                                  event(event::event_kind::integer, 18446744073709551615UL),
                                  event(event::event_kind::end)};
        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("One begin_dictionary, byte_string, integer, end should occur") {
                REQUIRE(dec.events == expect);
            }
        }
    }
}
