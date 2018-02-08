#include "utility/aspect.hpp"
#include "utility/thread_pool.hpp"
#include "utility/shared_ptr_wrapper.hpp"
#include "utility/function_wrapper.hpp"
#include <type_traits>
#include <iostream>
#include <typeinfo>
#include <unistd.h>
#include <utility>
#include <string>
#include <optional>
#include <functional>
#include <array>

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

int GT()
{
    cout << "GT function" << endl;
    return 5;
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
/*
template<typename Arg>
decltype(auto) wra()
{
    auto lmp([](Arg arg) {
        cout << arg << endl;
    });
    lmp(4);
    
    cout << typeid(Arg).name() << endl;
    function_wrapper<decltype(lmp)> func([&lmp]{return lmp;});
    cout << typeid(decltype(func)).name() << endl;
    return func;
}
*/


/*
class base
{
    using anyType = decltype(any);
public:
    anyType lmd;
    std::function<void>;
    template<typename T>
    base(const T& t)
    {
        [](){return lmd(t);}
        
    }

    template<typename T>
    T showType(const T& t)
    {
        return t;
    }
};
*/

auto any = [](auto&& obj) {return std::move(obj);};
using anyType = decltype(any);
auto anyObject = shared_ptr_wrapper<anyType>(any);


int main()
{
    auto lmp([]{
        cout << "I am lambda" << endl;   
       return string("zwq") ;
    });

    DD dd;
    shared_ptr_wrapper<DD> sp_D = std::make_shared<DD>(dd);
    //cout << typeid(decltype(&(*sp_D)::front)).name() << endl;
    //auto f = function_wrapper(std::bind(&DD::front,&dd));
    thread_pool thrp;

    //function_wrapper<int> func(GT);
    //cout << func() << endl;

    //function_wrapper<> gunc(std::bind(&DD::front,sp_D,5));
    //gunc();
    thrp.submit(GT);



    thrp.submit(lmp);
    //thrp.submit([&]{invoke_aspect<AA,DD>(std::bind(&DD::front,sp_D,std::placeholders::_1),5);});
    //std::function<void(void)> temp(&BB::front);
    
    //auto temp([&](int a) {return std::bind(&DD::front, &dd, a)();});
    //function_wrapper<> func_(&DD::front);
    //func_(sp_D)(5);
    /*auto ff = &DD::front;
    auto mff = std::mem_fn(&DD::front);
    auto gg = GT;
    //thrp.submit()
//    thrp.submit([&]{invoke_aspect<CC>(temp);});
    shared_ptr_wrapper<std::string> str1(typeid(std::string).name());
    //cout << *str1 << endl;
    using pkc = shared_ptr_wrapper<char>;
    //cout << typeid(decltype(typeid(decltype(thrp)).name())).name() << endl;
    cout << typeid(decltype(ff)).name() << endl;
    cout << typeid(decltype(mff)).name() << endl;
    cout << typeid(decltype(gg)).name() << endl;
    cout << std::is_member_pointer_v<decltype(ff)> << endl;
    cout << std::is_member_pointer_v<decltype(mff)> << endl;
    cout << std::is_member_pointer_v<decltype(gg)> << endl;
    cout << std::is_member_pointer_v<pkc> << endl;
    std::array<int, std::is_member_pointer_v<decltype(ff)>> arr;*/
    //cout << typeid(pkc).name() << endl;

    //cout << typeid(shared_ptr<A<int>>).name() << endl;

    std::string pkc(typeid(const char*).name());
    cout << "TYPEID: " << pkc << endl;
    
    auto lmpi([]() {return string("zwq");});
    //cout << typeid(decltype(lmpi)).name() << endl;
    //cout << typeid(decltype(lmp)).name() << endl;
    //thrp.submit([&]{invoke<BB,CC>(GT);});
    sleep(2);
    terminal(thrp);


    AA aa;

    auto a1 = anyObject;
    auto a2 = anyObject;
    auto b1 = (*a1)(AA());
    (*a1)(AA()).front(5);
    (*a2)(DD()).front(4);
    std::bind(&AA::front,&b1,8)();

    //anyObject(aa);
   // cout << typeid(decltype(anyObject)).name() << endl;
   // cout << typeid(decltype(anyObject(4))).name() << endl;
   // cout << typeid(decltype(anyObject("zwq"))).name() << endl;
   // cout << typeid(decltype(anyObject(AA()))).name() << endl;
   // cout << typeid(decltype(anyObject(DD()))).name() << endl;
    //wra()()(2);
    return 0;
}
