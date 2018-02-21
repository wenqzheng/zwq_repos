#include "shared_ptr_wrapper.hpp"
#include "hashmap_unique.hpp"
#include "object_tmp.hpp"
#include <iostream>
#include <string>
#include <utility>
#include <cstddef>
#include <type_traits>
using namespace std;

int f(int i)
{
    return i+5;
}

string g(int i)
{
    return "OK";
}

class AA
{
public:
    int a;
    AA(int i)
        :a(i)
    {}
};



int main()
{
    hashmap<string,int> hmap;
    hmap.insert("1",2);
    hmap.insert("2",4);
    hmap.insert("4",8);
    hmap.insert("4",32);
    hmap.insert("8",16);
    cout << hmap["4"] << endl;
    cout << hmap["4"] << endl;
    
    hmap.for_each([](std::pair<string,int>* spa) {
        cout << "(" << (*spa).first << ", " << (*spa).second << ")" << endl;
    });
    auto at = make_shared<AA>(5);
    cout << (*at).a << endl;
    shared_ptr_wrapper<int> sp_A = nullptr;
    auto iter = hmap.find("8");
    cout << hmap.find("4")->second << endl;

    IoCcontainer ioc;
    ioc.regeditType<AA,int>("AA");
    auto pa = ioc.resolve<AA>("AA",4);
    cout << pa->a << endl;
    AA* p_A;
    shared_ptr_wrapper<AA> sp_B = std::make_shared<AA>(8);
    p_A = &(*sp_B);
    cout << p_A->a << endl;
    
    //cout << *(obja.m_spw_obj) << endl;
    //cout << *(objb.m_spw_obj) << endl;
    return 0;
}
