#include "queue/blockingconcurrentqueue.h"
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <string>
#include <typeinfo>
using namespace std;

int main()
{
    cout << std::thread::hardware_concurrency() << endl;
    cout << std::this_thread::get_id() << endl;
    std::shared_ptr<std::string> Str;
    cout << atomic_is_lock_free(&Str) << endl;
    moodycamel::BlockingConcurrentQueue<std::shared_ptr<string>> q;
    std::thread producer([&]() {
        string str;
        cin >> str;
        std::shared_ptr<string> pi = make_shared<string>(str);
        q.enqueue(pi);
        cout << std::this_thread::get_id() << endl;
    });

    std::thread consumer([&]() {
        std::shared_ptr<string> item;
        q.wait_dequeue(item);
        cout << typeid(decltype(item)).name() << endl;
        cout << *item << endl;
        cout << std::this_thread::get_id() << endl;
    });

    producer.join();
    consumer.join();
    return 0;
}
