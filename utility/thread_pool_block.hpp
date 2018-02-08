// thread_pool.hpp
// ----by wenqzheng
// you should manipulate blocking in runtime
//-----------------------------------------------------------------------------

#pragma once

#include "../queue/blockingconcurrentqueue.h"
#include "function_wrapper.hpp"
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <future>
#include <type_traits>

class thread_pool
{
    using taskType = function_wrapper<>;
    static thread_local std::atomic_bool workThread;
    std::atomic_bool flag;
    std::vector<std::thread> threads_group;
    moodycamel::BlockingConcurrentQueue<taskType> task_queue;
    
    void run_pending_task()
    {
        taskType task;
        moodycamel::ConsumerToken ctok(task_queue);
        if (task_queue.try_dequeue(ctok, task) || task_queue.try_dequeue(task))
            task();
        else {
	        task_queue.wait_dequeue(task);
	        task();
        }
    }

    void work_thread()
    {
        workThread = true;
        while (!flag) {
            run_pending_task();
        }
    }

    friend void terminal(thread_pool& thrp);

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

    template<typename FuncType>
    decltype(auto) submit(FuncType func)
    {
        using result_type = std::invoke_result_t<FuncType>;
        std::packaged_task<result_type()> task(std::move(func));
        std::future<result_type> ret(task.get_future());
        if (workThread) {
            moodycamel::ProducerToken ptok(task_queue);
            task_queue.enqueue(ptok, std::move(task));
        } else
            task_queue.enqueue(std::move(task));
        return ret;
    }

};

thread_local std::atomic_bool thread_pool::workThread;

void terminal(thread_pool& thrp)
{
    for (auto i = 0; i < thrp.threads_group.capacity(); ++i)
        thrp.submit(std::bind(pthread_exit, nullptr));
}
