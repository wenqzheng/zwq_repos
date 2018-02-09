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
#include <vector>

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
using Object = shared_ptr_wrapper<anyType>;
/*
class object
{
public:
    template<typename T>
    decltype(auto) operator()(T&& t)
    {
        return (*anyObject)(t);
    }
};
*/
std::vector<Object> vecany;

#define REGKEY(KEY,...) \
enum KEY                \
{   \
__VA_ARGS__    \
}

//#define eenum(...) __VA_ARGS__


//REGKEY(NEWKEY,key_1,key_2,key_3);
REGKEY(key,key1,key2,key3);
REGKEY(oldkey,oldkey1,oldkey2,oldkey3);
/*
    enum NEWKEY {
        key_1,
        key_2,
        key_3
    };
*/

#define CONBINE(PREFIX,...)

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


    AA aa;
    vecany.push_back(std::make_shared<anyType>(any));
    (*vecany[0])(AA());
    vecany.push_back(std::make_shared<anyType>(any));
    (*vecany[1])(DD());

    auto k1 = key1;
    auto k2 = key2;
    auto ok1 = oldkey1;
    auto ok2 = oldkey2;


    (*vecany[k1])(BB()).front();
    (*vecany[ok1])(CC()).back();
    //object(AA()).front(5);
    //object(DD()).front(4);
   // object(aa).front(8);
//    std::bind(&AA::front,&b1,8)();

    //anyObject(aa);
    cout << typeid(decltype(k1)).name() << endl;
    cout << key1 << endl;
    cout << typeid(decltype(k2)).name() << endl;
    cout << key2 << endl;

    auto vglambda = [](auto printer) {
        return [=](auto&&... ts) {
            printer(std::forward<decltype(ts)>(ts)...);
            return [=]{return printer(ts...);};
        };
    };

    auto p = vglambda([](auto v1, auto v2, auto v3) {
            cout << v1 << v2 << v3 << endl;
            return string("zwq");
            });
    auto q = p(1,'a',3.14);

    thrp.submit(thrp.submit(std::bind(p,2,'b',2.78)).get());

    //f_w();


   cout << typeid(decltype(q)).name() << endl;
   cout << typeid(decltype(q())).name() << endl;
    

   auto rret = []{cout << "I am for test" << endl;};
   auto trret = [&]{return rret;};
   trret()();
   //trret()();
   // cout << typeid(decltype(anyObject(AA()))).name() << endl;
   // cout << typeid(decltype(anyObject(DD()))).name() << endl;
    //wra()()(2);
    return 0;
}
