// thread_pool.hpp
// ----by wenqzheng
// you should maintain running in runtime
//-----------------------------------------------------------------------------

#pragma once

#include "concurrentqueue.h"
#include "function_wrapper.hpp"
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <future>
#include <utility>
#include <type_traits>

class thread_pool
{
    using taskType = function_wrapper<>;
    static thread_local std::atomic_bool __workThread;
    std::atomic_bool __flag;
    std::vector<std::thread> threads_group;
    moodycamel::ConcurrentQueue<taskType> task_queue;
    
    void run_pending_task()
    {
        taskType task;
        moodycamel::ConsumerToken ctok(task_queue);
        if (task_queue.try_dequeue(ctok, task) || task_queue.try_dequeue(task))
            task();
        else
            std::this_thread::yield();
    }

    void work_thread()
    {
        __workThread = true;
        while (!__flag) {
            run_pending_task();
        }
    }

    friend void terminal(thread_pool& thrp);
 
public:
    thread_pool(unsigned int num = std::thread::hardware_concurrency())
        :__flag(false)
    {
        try {
            for (auto i = 0; i < num; ++i)
                threads_group.emplace_back(
		    std::thread(&thread_pool::work_thread, this));
	    threads_group.shrink_to_fit();
        } catch (...) {
            __flag = true;
            throw;
        }
    }

    ~thread_pool()
    {
        __flag = true;
        for (auto& thr:threads_group)
            if (thr.joinable())
                thr.join();
    }

    template<typename FuncType>
    decltype(auto) submit(FuncType func)
    {
        using result_type = std::invoke_result_t<FuncType>;
        std::packaged_task<result_type()> task(std::move(func));
        std::future<result_type> ret(task.get_future());
        if (__workThread) {
            moodycamel::ProducerToken ptok(task_queue);
            task_queue.enqueue(ptok, std::move(task));
        } else
            task_queue.enqueue(std::move(task));
            return ret;
    }

};

thread_local std::atomic_bool thread_pool::__workThread;

void terminal(thread_pool& thrp)
{
    for (auto i = 0; i < thrp.threads_group.capacity(); ++i)
        thrp.submit(std::bind(pthread_exit, nullptr));
}
