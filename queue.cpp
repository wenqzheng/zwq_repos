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


void lmp()
{
    int local = thread_pool::workThread;
    cout << "const std::string& " << endl;
    cout << "workThread: " << local << endl;
}
/*
void lmp(std::string&& str)
{
    cout << "std::string&& " << str << endl;
}
*/
/*
void lmp()
{
    cout << "zwq" << endl;
    cout << std::this_thread::get_id() << endl;
}
*/



int main()
{
    std::string str1 = "hello";
    auto str2 = std::string("world");
    thread_pool thrp(7);
    //thrp.submit(std::bind(lmp,str1)).get();
    thrp.submit([&](){
        int local = thrp.workThread;
        cout << "good" << endl;
        cout << "workThread: " << local << endl;
        cout << std::this_thread::get_id() << endl;
        thrp.submit(lmp).get();
    });
 //   cout << typeid(decltype(str1)).name() << endl;
 //   cout << typeid(decltype(str2)).name() << endl;
    //lmp("zwq");
    //lmp(std::move(str1));
    //lmp(std::move(str2));
    return 0;
}
