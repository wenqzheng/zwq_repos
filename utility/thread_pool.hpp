// thread_pool.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "../queue/blockingconcurrentqueue.h"
#include "function_wrapper.hpp"
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <functional>

using taskType
    // = std::shared_ptr<function_wrapper>;
	= std::function<void()>;
	// = function_wrapper;

class thread_pool
{
    std::atomic_bool flag;
    std::vector<std::thread> threads_group;
    moodycamel::BlockingConcurrentQueue<taskType> task_queue;

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

    template<typename FuncType>
    void submit(FuncType func)
    {
        task_queue.enqueue(func);
    }

    void run_pending_task()
    {
	    taskType task;
        task_queue.wait_dequeue(task);
        task();
    }

};
