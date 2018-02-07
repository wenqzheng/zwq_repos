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

static int i = 0;

void lmp()
{
    ++i;
    cout << "zwq" << endl;
    cout << std::this_thread::get_id() << endl;
}

int main()
{
    std::string str1 = "hello";
    auto str2 = std::string("world");
    thread_pool thrp(7);
    thrp.submit(lmp);
    thrp.submit([&](){
        cout << "good" << endl;
        cout << std::this_thread::get_id() << endl;
        thrp.submit(lmp);
    });
    sleep(5);
    cout << "i = " << i << endl;
 //   cout << typeid(decltype(str1)).name() << endl;
 //   cout << typeid(decltype(str2)).name() << endl;
    //lmp("zwq");
    //lmp(std::move(str1));
    //lmp(std::move(str2));
    return 0;
}
