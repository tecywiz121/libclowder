/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include <vector>
#include <sstream>

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
    }

    virtual void byte_string(std::string v) override
    {
        n_events++;
        events.emplace_back(event::event_kind::byte_string, v);
    }

    virtual void begin_list() override
    {
        n_events++;
        events.emplace_back(event::event_kind::begin_list);
    }

    virtual void begin_dictionary() override
    {
        n_events++;
        events.emplace_back(event::event_kind::begin_dictionary);
    }

    virtual void end() override
    {
        n_events++;
        events.emplace_back(event::event_kind::end);
    }
};

SCENARIO("A decoder doesn't have to implement any parsing functions")
{
    clowder::bencode::decoder dec;

    GIVEN("A complex structure to decode") {
        std::string data("ld4:spami10eee");
        WHEN("the data is parsed") {
            THEN("nothing should happen") {
                dec.parse(data);
            }
        }
    }
}

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

    GIVEN("Only the letter e") {
        std::string data("e");
        WHEN("the data is parsed") {
            THEN("an exception should be thrown") {
                REQUIRE_THROWS(dec.parse(data));
            }
        }
    }

    GIVEN("A malformed string") {
        std::string data("0a:aoeu");
        WHEN("the data is parsed") {
            THEN("an exception should be thrown") {
                REQUIRE_THROWS(dec.parse(data));
            }
        }
    }

    GIVEN("A zero length string in a list") {
        std::string data("l0:e");
        std::vector<event> expect{event(event::event_kind::begin_list),
                                  event(event::event_kind::byte_string, ""),
                                  event(event::event_kind::end)};
        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("We should get one string event inside a list") {
                REQUIRE(dec.events == expect);
            }
        }
    }

    GIVEN("A zero length string") {
        std::string data("0:");
        std::vector<event> expect{event(event::event_kind::byte_string, "")};
        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("We should get one string event") {
                REQUIRE(dec.events == expect);
            }
        }
    }

    GIVEN("A really big integer") {
        std::string data("i18446744073709551615e");
        std::vector<event> expect{event(event::event_kind::integer, 18446744073709551615UL)};
        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("We should get the same integer out") {
                REQUIRE(dec.events == expect);
            }
        }
    }

    GIVEN("A string length too long") {
        std::string data("28446744073709551615:boop");
        WHEN("the data is parsed") {
            THEN("We should get an exception") {
                REQUIRE_THROWS(dec.parse(data));
            }
        }
    }

    GIVEN("An oversized integer") {
        std::string data("i28446744073709551615e");
        WHEN("the data is parsed") {
            THEN("We should get an exception") {
                REQUIRE_THROWS(dec.parse(data));
            }
        }
    }

    GIVEN("An unterminated integer") {
        std::string data("i1");
        WHEN("the data is parsed") {
            THEN("We should get an exception") {
                REQUIRE_THROWS(dec.parse(data));
            }
        }
    }

    GIVEN("Garbage data") {
        std::string data("aaaaaaa");
        WHEN("the data is parsed") {
            dec.parse(data);
            THEN("We should get no events") {
                REQUIRE(dec.n_events == 0);
            }
        }
    }

    GIVEN("Garbage data that looks like a list") {
        std::string data("laaaaa");
        WHEN("the data is parsed") {
            THEN("We should get an exception") {
                REQUIRE_THROWS(dec.parse(data));
            }
        }
    }
}

SCENARIO("An encoder can encode data into a string")
{
    std::stringstream ss;
    clowder::bencode::encoder enc(ss);

    GIVEN("a zero length string") {
        WHEN("the data is encoded") {
            enc.byte_string("");
            THEN("a zero length string is encoded") {
                REQUIRE(ss.str() == "0:");
            }
        }
    }

    GIVEN("a string") {
        WHEN("the data is encoded") {
            enc.byte_string("hello world");
            THEN("the given string is encoded") {
                REQUIRE(ss.str() == "11:hello world");
            }
        }
    }

    GIVEN("one integer") {
        WHEN("the data is encoded") {
            enc.integer(10);
            THEN("one integer is encoded") {
                REQUIRE(ss.str() == "i10e");
            }
        }
    }

    GIVEN("an empty list") {
        WHEN("the data is encoded") {
            enc.begin_list();
            enc.end();
            THEN("an empty list should be encoded") {
                REQUIRE(ss.str() == "le");
            }
        }
    }

    GIVEN("nothing") {
        WHEN("calling end") {
            THEN("an exception should be thrown") {
                REQUIRE_THROWS(enc.end());
            }
        }
    }

    GIVEN("a dictionary") {
        enc.begin_dictionary();
        WHEN("the dictionary is ended") {
            enc.end();
            THEN("an empty dictionary should be encoded") {
                REQUIRE(ss.str() == "de");
            }
        }

        WHEN("an integer is added as a key") {
            THEN("an exception should be thrown") {
                REQUIRE_THROWS(enc.integer(0));
            }
        }

        WHEN("a list is added as a key") {
            THEN("an exception should be thrown") {
                REQUIRE_THROWS(enc.begin_list());
            }
        }

        WHEN("a dictionary is added as a key") {
            THEN("an exception should be thrown") {
                REQUIRE_THROWS(enc.begin_dictionary());
            }
        }

        WHEN("a byte_string is added as a key") {
            enc.byte_string("key");
            WHEN("the dictionary is ended") {
                THEN("an exception should be thrown") {
                    REQUIRE_THROWS(enc.end());
                }
            }

            WHEN("an integer is added as a value") {
                enc.integer(10);
                enc.end();
                THEN("a dictionary should be encoded") {
                    REQUIRE(ss.str() == "d3:keyi10ee");
                }
            }
        }

        WHEN("two byte_strings are added as keys in the wrong order") {
            enc.byte_string("z");
            enc.byte_string("z");
            THEN("an exception should be thrown") {
                REQUIRE_THROWS(enc.byte_string("a"));
            }
        }

        WHEN("two byte_strings are added as keys in the correct order") {
            enc.byte_string("a");
            enc.byte_string("b");
            enc.byte_string("c");
            enc.byte_string("d");
            enc.end();
            THEN("A dictionary with two keys should be added") {
                REQUIRE(ss.str() == "d1:a1:b1:c1:de");
            }
        }
    }
}

SCENARIO("a message is encoded, then decoded")
{
    std::stringstream ss;
    GIVEN("an encoder and a decoder") {
        test_decoder dec;
        clowder::bencode::encoder enc(ss);

        std::vector<event> expects{event(event::event_kind::begin_dictionary),
                                   event(event::event_kind::byte_string, "v1"),
                                   event(event::event_kind::begin_list),
                                   event(event::event_kind::integer, 87),
                                   event(event::event_kind::byte_string, "spam"),
                                   event(event::event_kind::begin_dictionary),
                                   event(event::event_kind::byte_string, "v2"),
                                   event(event::event_kind::integer, 44),
                                   event(event::event_kind::end),
                                   event(event::event_kind::end),
                                   event(event::event_kind::end)};

        WHEN("a complex message is encoded") {
            enc.begin_dictionary();
            enc.byte_string("v1");

            enc.begin_list();
            enc.integer(87);
            enc.byte_string("spam");

            enc.begin_dictionary();
            enc.byte_string("v2");
            enc.integer(44);
            enc.end();

            enc.end();

            enc.end();

            THEN("the same message is decoded") {
                dec.parse(ss.str());

                REQUIRE(dec.events == expects);
            }
        }
    }
}
