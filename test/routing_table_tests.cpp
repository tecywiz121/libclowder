/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include "../src/routing_table.hpp"

#include <iostream>

namespace clowder
{

class routing_table_pvt
{
private:
    const routing_table& _table;

public:
    routing_table_pvt(const routing_table& table) : _table(table) {}

    const std::array<routing_table::bucket_type, routing_table::n_buckets>&
        buckets() { return _table._buckets; }
};

}

SCENARIO("peers are added to routing tables", "[routing_table]")
{
    clowder::node_id my_id("00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF");

    clowder::node_id p1_id("8000000000000000000000000000000000000000000000000000000000000000");
    clowder::peer p1(p1_id, nullptr);

    clowder::node_id p2_id("4000000000000000000000000000000000000000000000000000000000000000");
    clowder::peer p2(p2_id, nullptr);

    clowder::node_id p3_id("4800000000000000000000000000000000000000000000000000000000000000");
    clowder::peer p3(p3_id, nullptr);

    GIVEN("an empty routing table") {
        clowder::routing_table table(my_id);
        clowder::routing_table_pvt pvt(table);

        WHEN("adding own node id") {
            clowder::peer me(my_id, nullptr);
            table.update(me);

            THEN("no new peers should be in the table") {
                CHECK(std::all_of(pvt.buckets().begin(),
                                  pvt.buckets().end(),
                                  [](decltype(pvt.buckets()[0])& b) { return b.size() == 0;}));
            }
        }

        WHEN("a new peer in bucket 1 is updated") {
            table.update(p2);

            THEN("a new peer should show up in bucket 1") {
                REQUIRE(pvt.buckets()[0].size() == 0);
                REQUIRE(pvt.buckets()[1].size() == 1);

                CHECK(std::all_of(pvt.buckets().begin() + 2,
                                  pvt.buckets().end(),
                                  [](decltype(pvt.buckets()[0])& b) { return b.size() == 0;}));

                REQUIRE(pvt.buckets()[1][0].id() == p2_id);
            }
        }

        WHEN("a new peer in bucket 0 is updated") {
            table.update(p1);

            THEN("a new peer should show up in bucket 0") {
                REQUIRE(pvt.buckets()[0].size() == 1);

                CHECK(std::all_of(pvt.buckets().begin() + 1,
                                  pvt.buckets().end(),
                                  [](decltype(pvt.buckets()[0])& b) { return b.size() == 0;}));

                REQUIRE(pvt.buckets()[0][0].id() == p1_id);
            }
        }

        WHEN("two new peers are updated") {
            table.update(p1);
            table.update(p2);

            THEN("a new peer should show up in buckets 0 and 1") {
                REQUIRE(pvt.buckets()[0].size() == 1);
                REQUIRE(pvt.buckets()[1].size() == 1);

                CHECK(std::all_of(pvt.buckets().begin() + 2,
                                  pvt.buckets().end(),
                                  [](decltype(pvt.buckets()[0])& b) { return b.size() == 0;}));

                REQUIRE(pvt.buckets()[0][0].id() == p1_id);
                REQUIRE(pvt.buckets()[1][0].id() == p2_id);
            }
        }
    }

    GIVEN("a routing table with two peers in different buckets") {
        clowder::routing_table table(my_id);
        clowder::routing_table_pvt pvt(table);

        table.update(p1);
        table.update(p2);

        WHEN("adding an existing peer") {
            table.update(p1);

            THEN("nothing should change") {
                REQUIRE(pvt.buckets()[0].size() == 1);
                REQUIRE(pvt.buckets()[1].size() == 1);

                CHECK(std::all_of(pvt.buckets().begin() + 2,
                                  pvt.buckets().end(),
                                  [](decltype(pvt.buckets()[0])& b) { return b.size() == 0;}));

                REQUIRE(pvt.buckets()[0][0].id() == p1_id);
                REQUIRE(pvt.buckets()[1][0].id() == p2_id);
            }
        }

        WHEN("adding a new peer to an occupied bucket") {
            table.update(p3);

            THEN("bucket 1 should have two peers") {
                REQUIRE(pvt.buckets()[0].size() == 1);
                REQUIRE(pvt.buckets()[1].size() == 2);

                CHECK(std::all_of(pvt.buckets().begin() + 2,
                                  pvt.buckets().end(),
                                  [](decltype(pvt.buckets()[0])& b) { return b.size() == 0;}));

                REQUIRE(pvt.buckets()[0][0].id() == p1_id);
                REQUIRE(pvt.buckets()[1][0].id() == p2_id);
                REQUIRE(pvt.buckets()[1][1].id() == p3_id);
            }
        }
    }

    GIVEN("a routing table with two peers in the same bucket") {
        clowder::routing_table table(my_id);
        clowder::routing_table_pvt pvt(table);

        table.update(p2);
        table.update(p3);

        WHEN("adding the oldest peer again") {
            table.update(p2);

            THEN("the most recently added peer should be at the back of the bucket") {
                REQUIRE(pvt.buckets()[1].size() == 2);
                REQUIRE(pvt.buckets()[1][0].id() == p3_id);
                REQUIRE(pvt.buckets()[1][1].id() == p2_id);
            }
        }

        WHEN("adding the newest peer again") {
            table.update(p1);

            THEN("the most recently added peer should be at the back of the bucket") {
                REQUIRE(pvt.buckets()[1].size() == 2);
                REQUIRE(pvt.buckets()[1][0].id() == p2_id);
                REQUIRE(pvt.buckets()[1][1].id() == p3_id);
            }
        }
    }
}

SCENARIO("a populated routing table is searched for a node_id")
{
    clowder::node_id my_id("0000000000000000000000000000000000000000000000000000000000000000");
    clowder::routing_table table(my_id);
    clowder::routing_table_pvt pvt(table);

    std::vector<clowder::peer> peers;
    peers.reserve(clowder::node_id::length);

    for (size_t ii = 0; ii < clowder::node_id::length; ii++) {
        for (size_t jj = 255; jj > 0; jj>>=1) {
            std::stringstream ss;
            for (size_t kk = 0; kk < ii; kk++) {
                ss << "00";
            }

            ss << std::setfill('0')
               << std::setw(sizeof(uint8_t)*2)
               << std::hex;


            ss << (jj & 0xFF);

            for (size_t kk = 0; kk+1 < clowder::node_id::length-ii; kk++) {
                ss << "FF";
            }

            clowder::peer p(clowder::node_id(ss.str()), nullptr);
            table.update(p);
            peers.push_back(p);
        }
    }

    GIVEN("the table's own node id") {
        WHEN("zero nodes are searched for") {
            std::vector<clowder::peer> results = table.closest(my_id, 0);

            THEN("exactly no nodes should be returned") {
                REQUIRE(results.size() == 0);
            }
        }

        WHEN("one node is searched for") {
            std::vector<clowder::peer> results = table.closest(my_id, 1);
            clowder::node_id last("0000000000000000000000000000000000000000000000000000000000000001");

            THEN("exactly one peer should be returned") {
                REQUIRE(results.size() == 1);
                REQUIRE(results[0].id() == last);
            }
        }

        WHEN("two nodes are searched for") {
            std::vector<clowder::peer> results = table.closest(my_id, 2);
            clowder::node_id last("0000000000000000000000000000000000000000000000000000000000000001");
            clowder::node_id before("0000000000000000000000000000000000000000000000000000000000000003");

            THEN("exactly two peers should be returned") {
                REQUIRE(results.size() == 2);
                CHECK(results[0].id() == last);
                CHECK(results[1].id() == before);
            }
        }
    }

    GIVEN("the furthest node id") {
        clowder::node_id target("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        clowder::node_id close("7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        WHEN("one node is searched for") {
            std::vector<clowder::peer> results = table.closest(target, 1);

            THEN("exactly one node should be returned") {
                REQUIRE(results.size() == 1);
                CHECK(results[0].id() == target);
            }
        }

        WHEN("two nodes are searched for") {
            std::vector<clowder::peer> results = table.closest(target, 2);

            THEN("exactly two nodes should be returned") {
                REQUIRE(results.size() == 2);
                CHECK(results[0].id() == target);
                CHECK(results[1].id() == close);
            }
        }
    }

    GIVEN("a middle node id") {
        clowder::node_id target ("00000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        clowder::node_id closest("000000000000000000000000000000007FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        clowder::node_id closer ("00000000000000000000000000000001FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        clowder::node_id close  ("000000000000000000000000000000003FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");

        WHEN("one node is searched for") {
            std::vector<clowder::peer> results = table.closest(target, 1);

            THEN("exactly one node should be returned") {
                REQUIRE(results.size() == 1);
                CHECK(results[0].id() == target);
            }
        }

        WHEN("two nodes are searched for") {
            std::vector<clowder::peer> results = table.closest(target, 2);

            THEN("exactly two nodes should be returned") {
                REQUIRE(results.size() == 2);
                CHECK(results[0].id().to_string() == target.to_string());
                CHECK(results[1].id().to_string() == closest.to_string());
            }
        }

        WHEN("three nodes are searched for") {
            std::vector<clowder::peer> results = table.closest(target, 3);

            THEN("exactly three nodes should be returned") {
                REQUIRE(results.size() == 3);
                CHECK(results[0].id().to_string() == target.to_string());
                CHECK(results[1].id().to_string() == closest.to_string());
                CHECK(results[2].id().to_string() == closer.to_string());
            }
        }

        WHEN("four nodes are searched for") {
            std::vector<clowder::peer> results = table.closest(target, 4);

            THEN("exactly four nodes should be returned") {
                REQUIRE(results.size() == 4);
                CHECK(results[0].id().to_string() == target.to_string());
                CHECK(results[1].id().to_string() == closest.to_string());
                CHECK(results[2].id().to_string() == close.to_string());
                CHECK(results[3].id().to_string() == closer.to_string());
            }
        }

        WHEN("more than all the nodes are searched for") {
            std::vector<clowder::peer> results = table.closest(target, 257);

            THEN("exactly 255 nodes should be returned") {
                REQUIRE(results.size() == peers.size());
                // TODO - SW: Check the ordering
            }
        }
    }
}
