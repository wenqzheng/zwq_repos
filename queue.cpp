#include "queue/blockingconcurrentqueue.h"
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <string>
#include <typeinfo>
using namespace std;

using valueType = std::unique_ptr<std::string>;// std::shared_ptr<>

int main()
{
    cout << std::thread::hardware_concurrency() << endl;
    cout << std::this_thread::get_id() << endl;
    std::shared_ptr<std::string> Str;
    cout << atomic_is_lock_free(&Str) << endl;
    moodycamel::BlockingConcurrentQueue<valueType> q;
    std::thread producer([&]() {
        string str;
        cin >> str;
        valueType pi = make_unique<string>(str);
        q.enqueue(std::move(pi));
        cout << std::this_thread::get_id() << endl;
    });

    std::thread consumer([&]() {
        valueType item;
        q.wait_dequeue(item);
        cout << typeid(decltype(item)).name() << endl;
        cout << *item << endl;
        cout << std::this_thread::get_id() << endl;
    });

    producer.join();
    consumer.join();
    return 0;
}
