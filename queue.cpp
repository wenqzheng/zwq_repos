#include "queue/blockingconcurrentqueue.h"
#include "include/Strategy.hpp"
#include "utility/thread_pool.hpp"
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <string>
#include <typeinfo>
#include <vector>
#include <unistd.h>
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

void lmp(const std::string& str)
{
    cout << "const std::string& " << str << endl;
}

void lmp(std::string&& str)
{
    cout << "std::string&& " << str << endl;
}

int main()
{
    moodycamel::BlockingConcurrentQueue<valueType> q;
    std::thread producer([&]() {
        int i;
	    cin >> i;
	    Stra1 s1;
	    Stra2 s2;
	    Stra3 s3;
	    valueType pi;
	    if (i == 1)
            pi = std::make_shared<Strategy>(s1);
	    else if (i == 2)
	        pi = std::make_shared<Strategy>(s2);
	    else if (i == 3)
     	    pi = std::make_shared<Strategy>(s3);
	    else {
	        cout << "error num" << endl;
	        return;
	    }
        q.enqueue(pi);
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
 
    //thread_pool thrp(7);
    //thrp.submit(lmp);
    const std::string str1 = "zwq";
    auto str2 = std::string("zwq");
    cout << typeid(decltype(str1)).name() << endl;
    cout << typeid(decltype(str2)).name() << endl;
    //lmp("zwq");
    lmp(std::move(str1));
    lmp(std::move(str2));
    return 0;
}
