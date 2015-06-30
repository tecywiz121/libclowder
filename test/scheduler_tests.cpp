/* vim: et sw=4 sts=4 ts=4 : */
#include "catch.hpp"

#include "../src/scheduler.hpp"

#include <thread>

using clowder::scheduler;

SCENARIO("a scheduler can have events added, cancelled, and executed")
{
    scheduler sched;

    size_t count = 0;
    std::function<void()> task_func = [&count] () { count++; };

    size_t count2 = 0;
    std::function<void()> task_func2 = [&count2] () { count2++; };

    WHEN("An event with a zero duration is added") {
        sched.execute_in(scheduler::duration::zero(),
                         task_func,
                         0);
        THEN("The task should be executed during the next periodic") {
            CHECK(sched.periodic() == scheduler::duration::max());
            REQUIRE(count == 1);

            CHECK(sched.periodic() == scheduler::duration::max());
            CHECK(count == 1);
        }
    }

    WHEN("Two events with zero durations are added") {
        sched.execute_in(scheduler::duration::zero(),
                         task_func,
                         0);
        sched.execute_in(scheduler::duration::zero(),
                         task_func,
                         0);
        THEN("Both tasks should be executed during the next periodic") {
            CHECK(sched.periodic() == scheduler::duration::max());
            REQUIRE(count == 2);

            CHECK(sched.periodic() == scheduler::duration::max());
            CHECK(count == 2);
        }
    }

    WHEN("Two events (one zero, and one not) are added") {
        sched.execute_in(std::chrono::milliseconds(100), task_func2, 0);
        sched.execute_in(scheduler::duration::zero(), task_func, 0);

        THEN("The zero should come first, followed later by the other") {
            scheduler::duration wait = sched.periodic();
            CHECK(wait < scheduler::duration::max());
            REQUIRE(count == 1);
            REQUIRE(count2 == 0);

            std::this_thread::sleep_for(wait);

            CHECK(sched.periodic() == scheduler::duration::max());
            REQUIRE(count == 1);
            REQUIRE(count2 == 1);
        }
    }

    WHEN("Two events (both non-zero) are added") {
        sched.execute_in(std::chrono::milliseconds(100), task_func2, 0);
        sched.execute_in(std::chrono::milliseconds(200), task_func, 0);

        THEN("Then the 1st call should not invoke anything, but the second and third should") {
            scheduler::duration wait = sched.periodic();
            CHECK(wait < scheduler::duration::max());
            REQUIRE(count == 0);
            REQUIRE(count2 == 0);

            std::this_thread::sleep_for(wait);

            wait = sched.periodic();
            CHECK(wait < scheduler::duration::max());
            REQUIRE(count == 0);
            REQUIRE(count2 == 1);

            std::this_thread::sleep_for(wait);

            CHECK(sched.periodic() == scheduler::duration::max());
            REQUIRE(count == 1);
            REQUIRE(count2 == 1);
        }
    }

    WHEN("An event is added, allowing the random calculation") {
        sched.execute_in(std::chrono::milliseconds(100),
                         task_func,
                         std::chrono::milliseconds(100).count());

        THEN("The first call should return 100 or 101 milliseconds of delay") {
            scheduler::duration wait = sched.periodic();
            REQUIRE(wait >= std::chrono::milliseconds(99));
            REQUIRE(wait <= std::chrono::milliseconds(102));
        }
    }

    WHEN("An event is added, then cancelled") {
        sched.execute_in(scheduler::duration::zero(),
                         task_func,
                         0)->cancel();
        THEN("The call should never be executed") {
            REQUIRE(sched.periodic() == scheduler::duration::max());
            REQUIRE(count == 0);
        }
    }
}

SCENARIO("An event is added far in the future") {
    scheduler sched;

    size_t count = 0;
    std::function<void()> task_func = [&count] () { count++; };

    size_t count2 = 0;
    std::function<void()> task_func2 = [&count2] () { count2++; };

    sched.execute_in(std::chrono::hours(87660), // ~10 years
                     task_func,
                     0);

    WHEN("The remaining time is checked") {
        scheduler::duration remaining = sched.remaining();

        THEN("There should be about the correct amount of time remaining") {
            REQUIRE(remaining >= std::chrono::hours(87659));
            REQUIRE(remaining < std::chrono::hours(87660));
        }
    }
}
