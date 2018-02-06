#include "queue/blockingconcurrentqueue.h"
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
using namespace std;

int main()
{
    cout << std::thread::hardware_concurrency() << endl;
    cout << std::this_thread::get_id() << endl;
    std::shared_ptr<std::string> Str;
    cout << atomic_is_lock_free(&Str) << endl;
    moodycamel::BlockingConcurrentQueue<int> q;
    std::thread producer([&]() {
        int i = 0;
        cin >> i;
        q.enqueue(i);
        cout << std::this_thread::get_id() << endl;
    });

    std::thread consumer([&]() {
        int item;
        q.wait_dequeue(item);
        cout << item+10 << endl;
        cout << std::this_thread::get_id() << endl;
    });

    producer.join();
    consumer.join();
    return 0;
}
