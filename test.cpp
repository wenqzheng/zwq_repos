#include "utility/aspect_abstract.hpp"
#include "utility/thread_pool_block.hpp"
#include "utility/smart_ptr_wrapper.hpp"
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
#include <deque>
#include <unordered_map>
#include <any>
#include <variant>

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
    int a;
    int b;
    int c;
    shared_ptr_wrapper<int> spi;
    AA(int _a, int _b, int _c, shared_ptr_wrapper<int> _spi):
        a(_a), b(_b), c(_c), spi(_spi)
    {}
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

class SP1
{
public:
    shared_ptr_wrapper<int> SP2;
    shared_ptr_wrapper<string> SP3;
};

int main()
{
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
    

    shared_ptr<int> outyn = make_shared<int>(8);

    
    cout << &outyn << "  "<< outyn.get() << endl;
    outyn = make_shared<int>(18);
    cout << &outyn << "  " << outyn.get() << endl;
sleep(2);    
std::allocator<int> __alloc;
cout << sizeof(__alloc) << endl;
auto tmp1 = std::allocate_shared<int>(__alloc,8);
auto tmp2 = std::allocate_shared<int>(__alloc,16);
auto tmp3 = std::allocate_shared<int>(__alloc,32);
auto tmp4 = std::allocate_shared<int>(__alloc,64);
cout << typeid(decltype(tmp1)).name() << endl;
cout << "&tmp1: " << &tmp1 << endl;
cout << "tmp1.get: " << tmp1.get() << endl;
cout << "*tmp1: " << *tmp1 << endl;
cout << typeid(decltype(tmp2)).name() << endl;
cout << "&tmp2: " << &tmp2 << endl;
cout << "tmp2.get: " << tmp2.get() << endl;
cout << "*tmp2: " << *tmp2 << endl;
cout << typeid(decltype(tmp3)).name() << endl;
cout << "&tmp3: " << &tmp3 << endl;
cout << "tmp3.get: " << tmp3.get() << endl;
cout << "*tmp3: " << *tmp3 << endl;
cout << typeid(decltype(tmp4)).name() << endl;
cout << "&tmp4: " << &tmp4 << endl;
cout << "tmp4.get: " << tmp4.get() << endl;
cout << "*tmp4: " << *tmp4 << endl;
auto ent1 = __alloc.allocate(1);__alloc.construct(ent1,3);
auto ent2 = __alloc.allocate(1);__alloc.construct(ent2,4);
auto ent3 = __alloc.allocate(1);__alloc.construct(ent3,5);
auto ent4 = __alloc.allocate(1);__alloc.construct(ent4,6);
cout << typeid(decltype(ent1)).name() << endl;
cout << "&ent1: " << &ent1 << endl;
cout << "ent1.get: " << ent1 << endl;
cout << "*ent1: " << *ent1 << endl;
cout << typeid(decltype(ent2)).name() << endl;
cout << "&ent2: " << &ent2 << endl;
cout << "ent2.get: " << ent2 << endl;
cout << "*ent2: " << *ent2 << endl;
cout << typeid(decltype(ent3)).name() << endl;
cout << "&ent3: " << &ent3 << endl;
cout << "ent3.get: " << ent3 << endl;
cout << "*ent3: " << *ent3 << endl;
cout << typeid(decltype(ent4)).name() << endl;
cout << "&ent4: " << &ent4 << endl;
cout << "ent4.get: " << ent4 << endl;
cout << "*ent4: " << *ent4 << endl;


variant<int,string> a("zwq");
string s = "OK";
variant<int,string> b = variant<int, string>(s);
cout << b.index() << endl;

shared_ptr_wrapper<SP1> sp1 = make_shared<SP1>();
sp1->SP2.store(shared_ptr_wrapper<int>(make_shared<int>(88)));
cout << *(sp1->SP2) << endl;
sp1->SP2.store(shared_ptr_wrapper<int>(make_shared<int>(128)));
cout << *(sp1->SP2) << endl;
atomic_bool atbl = true;
cout << atbl << endl;

int aaa = 88;
int bbb = 44;

const void* convoid;

shared_ptr<int> sppint(&aaa);// = shared_ptr<int>(&aaa);
cout << &aaa << endl;
cout << sppint.get() << endl;
cout << sppint.use_count() << endl;
sppint = shared_ptr<int>(&bbb);
cout << &bbb << endl;
cout << sppint.get() << endl;
cout << sppint.use_count() << endl;
cout << &aaa << endl;
cout << bbb << endl;
cout << *sppint << endl;
return 0;
}
