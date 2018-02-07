// thread_pool.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "../queue/concurrentqueue.h"
#include "function_wrapper.hpp"
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <future>
#include <type_traits>
#include <utility>

class thread_pool
{
    using taskType = function_wrapper;
    std::atomic_bool flag;
    std::vector<std::thread> threads_group;
    moodycamel::ConcurrentQueue<taskType> task_queue;

public:
    thread_pool(unsigned int num = std::thread::hardware_concurrency())
        :flag(false)
    {
        try {
            for (auto i = 0; i < num; ++i)
                threads_group.emplace_back(
		    std::thread(&thread_pool::work_thread, this));
	    threads_group.shrink_to_fit();
        } catch (...) {
            flag = true;
            throw;
        }
    }

    ~thread_pool()
    {
        flag = true;
        for (auto& thr:threads_group)
            if (thr.joinable())
                thr.join();
    }

    void work_thread()
    {
        while (!flag) {
            run_pending_task();
        }
    }
/*
    template<typename FuncType>
    void submit(FuncType func)
    {
        task_queue.enqueue(std::move(func));
    }
*/
    template<typename FuncType>
    decltype(auto) submit(FuncType func)
    {
        using result_type = typename std::result_of<FuncType()>::type;
        std::packaged_task<result_type()> task(std::move(func));
        std::future<result_type> ret(task.get_future());
        task_queue.enqueue(std::move(task));
        return ret;
    }

    void run_pending_task()
    {
	    taskType task;
        if (task_queue.try_dequeue(task))
            task();
        else
            std::this_thread::yield();
    }

};
