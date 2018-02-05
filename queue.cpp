#include "queue/blockingconcurrentqueue.h"
#include "include/Strategy.hpp"
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <string>
#include <typeinfo>
using namespace std;

class Stra1
{
public:
    void do_something() const
    {
        cout << "num 1 Strategy" << endl;
    }
};

class Stra2
{
public:
    void do_something() const
    {
        cout << "num 2 Strategy" << endl;
    }
};

class Stra3
{
public:
    void do_something() const
    {
        cout << "num 3 Strategy" << endl;
    }
};

template<int I>
class Stra
{
public:
    void do_something() const
    {
        cout << "num " << I << " Strategy" << endl;
    }
};

using valueType =  std::shared_ptr<Strategy>;

int main()
{
    cout << std::thread::hardware_concurrency() << endl;
    cout << std::this_thread::get_id() << endl;
    std::shared_ptr<std::string> Str;
    cout << atomic_is_lock_free(&Str) << endl;
    moodycamel::BlockingConcurrentQueue<valueType> q;
    std::thread producer([&]() {
        int i;
	cin >> i;
	Stra1 s1;
	Stra2 s2;
	Stra3 s3;
	valueType pi;
	if (i == 1)
            pi = make_shared<Strategy>(s1);
	else if (i == 2)
	    pi = make_shared<Strategy>(s2);
	else if (i == 3)
     	    pi = make_shared<Strategy>(s3);
	else {
	    cout << "error num" << endl;
	    return;
	}
        q.enqueue(std::move(pi));
        cout << std::this_thread::get_id() << endl;
    });

    std::thread consumer([&]() {
        valueType item;
        q.wait_dequeue(item);
        //cout << typeid(decltype(item)).name() << endl;
	item->do_something();
	cout << item->return_something() << endl;
        //cout << *item << endl;
        cout << std::this_thread::get_id() << endl;
    });

    producer.join();
    consumer.join();
    return 0;
}
