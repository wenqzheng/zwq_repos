#include "utility/aspect.hpp"
#include "utility/thread_pool.hpp"
#include "utility/shared_ptr_wrapper.hpp"
#include "utility/function_wrapper.hpp"
#include <iostream>
#include <typeinfo>
#include <unistd.h>
#include <utility>
#include <string>
#include <optional>
#include <functional>


using namespace std;

struct AA
{
    void front(int a)
    {
        cout << "front from AA: " << a << endl;
    }

    void back(int a)
    {
        cout << "back from AA: " << a << endl;
    }
};

struct BB
{
    void front()
    {
        cout << "front from BB" << endl;
    }
};

struct CC
{
    void back()
    {
        cout << "back from CC" << endl;
    }
};

struct DD
{
    void front(int a)
    {
        cout << "front from DD: " << a << endl;
    }

    void back(int a)
    {
        cout << "back from DD: " << a << endl;
    }
};

void GT()
{
    cout << "GT function" << endl;
}
template<typename T>
class A
{
};
template<typename T>
class B:public A<T>
{};
template<typename T>
class C:public A<T>
{};

template<typename Arg>
decltype(auto) wra()
{
    auto lmp([](Arg arg) {
        cout << arg << endl;
    });
    
    cout << typeid(Arg).name() << endl;
    function_wrapper<decltype(lmp)> func([&lmp]{return lmp;});
    cout << typeid(decltype(func)).name() << endl;
    return func;
}

int main()
{
    auto lmp([]{
        cout << "I am lambda" << endl;   
       return string("zwq") ;
    });

    shared_ptr_wrapper<BB> sp_D = std::make_shared<BB>();
    DD dd;
    //cout << typeid(decltype(&(*sp_D)::front)).name() << endl;
    //auto f = function_wrapper(std::bind(&DD::front,&dd));
    thread_pool thrp;
    //thrp.submit(lmp);
    //thrp.submit([&]{invoke_aspect<AA,DD>(std::bind(&DD::front,sp_D,std::placeholders::_1),5);});
    //std::function<void(void)> temp(&BB::front);
    function_wrapper<> temp(std::bind(&BB::front, sp_D));
//    thrp.submit([&]{invoke_aspect<CC>(temp);});
    shared_ptr_wrapper<std::string> str1(typeid(std::string).name());
    cout << *str1 << endl;
    using pkc = shared_ptr_wrapper<char>;
    //cout << typeid(decltype(typeid(decltype(thrp)).name())).name() << endl;
    //cout << typeid(decltype(str1)).name() << endl;
    //cout << typeid(pkc).name() << endl;

    //cout << typeid(shared_ptr<A<int>>).name() << endl;

    auto lmpi([]() {return string("zwq");});
    //cout << typeid(decltype(lmpi)).name() << endl;
    //cout << typeid(decltype(lmp)).name() << endl;
    //thrp.submit([&]{invoke<BB,CC>(GT);});
    sleep(2);
    wra<int>()()(2);
    return 0;
}
