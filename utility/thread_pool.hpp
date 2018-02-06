// thread_pool.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "../queue/blockingconcurrentqueue.h"
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <functional>

class thread_pool
{
public:
    std::atomic_bool flag;
    std::vector<std::thread> threads_group;
    moodycamel::BlockingConcurrentQueue<std::function<void()>> task_queue;

public:
    thread_pool(unsigned int num = std::thread::hardware_concurrency())
        :flag(false)
    {
        try {
            for (auto i = 0; i < num; ++i)
                threads_group.push_back(
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
        std::function<void()> task;
        task_queue.wait_dequeue(task);
        task();
    }

};
