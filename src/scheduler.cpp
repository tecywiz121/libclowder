/* vim: et sw=4 sts=4 ts=4 : */
#include "scheduler.hpp"

#include <array>
#include <algorithm>

using std::array;
using std::function;
using std::shared_ptr;

namespace clowder
{

scheduler::scheduler()
    : _rng(std::random_device()())  // Seed the pseudo-random generator
{

}

shared_ptr<task_handle> scheduler::execute_in(duration d,
                                              function<void()> fn,
                                              rep rand_div)
{
    time_point start = std::chrono::steady_clock::now() + d;

    // Randomize all events by a small amount to keep things from bunching up,
    // passing zero disables the randomization.
    if (0 < rand_div) {
        rep upper = d.count() / rand_div;
        upper = std::max(upper, static_cast<rep>(1));
        std::uniform_int_distribution<rep> dist(0, upper);
        rep offset = dist(_rng);
        start += duration(offset);
    }

    task t(start, fn);

    auto handle = t.handle();
    _tasks.push(std::move(t));

    return handle;
}

scheduler::duration scheduler::periodic()
{
    time_point now = std::chrono::steady_clock::now();
    duration retval = duration::max();

    while (!_tasks.empty()) {
        const task& current = _tasks.top();

        if (current.start() > now) {
            // If the top element's start time is in the future, we can exit
            retval = current.start() - now;
            break;
        }

        task_handle& handle = *current.handle();
        if (!handle._completed.exchange(true)) {
            // If the previous value of completed was false, this task hasn't
            // been cancelled.
            current.action()();
        }

        _tasks.pop();
    }

    return retval;
}

}
