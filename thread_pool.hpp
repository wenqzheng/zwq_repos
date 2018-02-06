// thread_pool.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "queue/blockingconcurrentqueue.h"
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <functional>
class function_wrapper
{
struct impl_base
{
    virtual void call() = 0;
    virtual ~impl_base() {}
};

template<typename Func>
struct impl_type:impl_base
{
    impl_type(Func&& func_):func(std::move(func_)) {}
    void call() {func();}
private:
    Func func;
};

std::unique_ptr<impl_base> impl;

public:
function_wrapper() = default;
function_wrapper(const function_wrapper&) = delete;
function_wrapper(function_wrapper&) = delete;
function_wrapper& operator=(const function_wrapper&) = delete;

template<typename Func>
function_wrapper(Func&& func)
    :impl(std::make_unique<impl_type<Func>>(std::move(func)))
{}

function_wrapper(function_wrapper&& func_)
    :impl(std::move(func_.impl))
{}

function_wrapper& operator=(function_wrapper&& func_)
{
    impl = std::move(func_.impl);
    return *this;
}

void operator()()
{
    impl->call();
}
};

class thread_pool
{
    std::atomic_bool flag;
    const int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads_group;
    moodycamel::BlockingConcurrentQueue<std::function<void()>> task_queue;

public:
    thread_pool()
        :flag(false)
    {
        try {
            for (int i = 0; i < thread_count; ++i)
                threads_group.push_back(
                    std::thread(&thread_pool::work_thread, this));
        } catch (...) {
            flag = true;
            throw;
        }
    }

    ~thread_pool()
    {
        flag = true;
        for (auto i = 0; i < thread_count; ++i)
            if (threads_group[i].joinable())
                threads_group[i].join();
    }

    void work_thread()
    {
        while (!flag) {
            run_pending_task();
        }
    }

    template<typename FuncType>
    void submit(FuncType func)
    {
        task_queue.enqueue(func);
    }

    void run_pending_task()
    {
        std::function<void()> task;
        task_queue.wait_dequeue(task);
        task();
    }

};
