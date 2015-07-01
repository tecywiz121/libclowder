/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_SCHEDULER_HPP
#define CLOWDER_SCHEDULER_HPP

#include <atomic>
#include <memory>
#include <chrono>
#include <functional>
#include <queue>
#include <random>

namespace clowder
{

class task_handle
{
    friend class scheduler;
private:
    std::atomic_bool _completed;

public:
    task_handle() = default;
    task_handle(task_handle&&) = delete;
    task_handle(const task_handle&) = delete;

    task_handle& operator=(task_handle&&) = delete;
    task_handle& operator=(const task_handle&) = delete;

    bool completed() const { return _completed; }
    void cancel() { _completed.store(true); } // TODO - SW: Remove cancelled elements from the queue
};

class scheduler
{
public:
    typedef std::chrono::steady_clock::duration duration;
    typedef std::chrono::steady_clock::rep rep;
    typedef std::chrono::steady_clock::period period;
    typedef std::chrono::steady_clock::time_point time_point;

    scheduler();
    std::shared_ptr<task_handle> execute_in(duration d,
                                            std::function<void()> f,
                                            rep rand_div = 20);

    duration periodic();
    duration remaining() const;
private:
    class task
    {
    private:
        time_point _start;
        std::shared_ptr<task_handle> _handle;
        std::function<void()> _action;

    public:
        task(time_point start, std::function<void()> action)
            : _start(start),
              _handle(std::make_shared<task_handle>()),
              _action(std::move(action))
        {

        }

        const time_point& start() const { return _start; }
        const std::shared_ptr<task_handle>& handle() const { return _handle; }
        const std::function<void()>& action() const { return _action; }
        bool operator>(const task& b) const { return _start > b._start; }
    };

    std::mt19937 _rng;
    std::priority_queue<task, std::vector<task>, std::greater<task>> _tasks;
};

}

#endif /* CLOWDER_SCHEDULER_HPP */

