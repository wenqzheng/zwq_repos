#include "utility/aspect_abstract.hpp"
#include "utility/thread_pool_block.hpp"
#include "utility/shared_ptr_wrapper.hpp"
#include "utility/function_wrapper.hpp"
#include "utility/hashmap_multiple.hpp"
#include <atomic>
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
#include <unordered_map>

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

template<typename T>
class arr
{
    public:
        T num[18];
};

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

    hashmap<int,int> hmap;
    for(auto i = 0; i < 5000; ++i)
    thrp.submit([&,i]{
        hmap.insert(i,i*2);
        });
    thrp.submit([&]{cout << hmap.size() << endl; cout << hmap.size() << endl;});
    for(auto i = 5000; i < 10000; ++i)
    thrp.submit([&,i]{
        hmap.insert(i,i*2);
        });
    thrp.submit([&]{cout << hmap.size() << endl; cout << hmap.size() << endl;});


    for(auto i = 15000; i < 20000; ++i)
    thrp.submit([&,i]{
        hmap.insert(i,i*2);
        });
    thrp.submit([&]{cout << hmap.size() << endl; cout << hmap.size() << endl;});
    for(auto i = 10000; i < 15000; ++i)
    thrp.submit([&,i]{
        hmap.insert(i,i*2);
        });
    thrp.submit([&]{cout << hmap.size() << endl; cout << hmap.size() << endl;});
    thrp.submit([&]{cout << hmap.find(2)->second << endl;});
    thrp.submit([&]{cout << hmap.find(2)->second << endl;});
    thrp.submit([&]{cout << hmap.size() << endl;});
    thrp.submit([&]{cout << hmap.find(2000)->second << endl;});
    cout << "thread id:" <<  this_thread::get_id() << endl;
    cout << "pthread_self: " << pthread_self() << endl;

    cout << "sizeof size_t: " << sizeof(std::size_t) << endl;
    union {
        std::uint64_t val;
        std::uint16_t tag[4];
    } tmp;

    tmp.val = 0xffeeddccbbaa9988;
    
    cout << hex << tmp.tag[0] << dec << endl;
    cout << hex << tmp.tag[1] << dec << endl;
    cout << hex << tmp.tag[2] << dec << endl;
    cout << hex << tmp.tag[3] << dec << endl;
    

    
sleep(5);

    auto lmd = [](auto&& t) {return std::forward<decltype(t)>(t);};
    AA aa;
    cout << &lmd << endl;
    cout << &aa << endl;
    cout << sizeof(lmd) << endl;

    std::atomic<arr<char>> bt; 
    cout << "is lock free ? " << atomic_is_lock_free(&bt) << endl;
   
    __uint128_t a;

    //std::atomic_uint128_t a;
    
    return 0;
}
