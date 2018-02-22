#include "shared_ptr_wrapper.hpp"
#include "hashmap_unique.hpp"
#include "IoCcontainer.hpp"
#include "aspect_entity.hpp"
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


struct DD
{
void front()
{
    cout << "I am zwq" << endl;
}

void back()
{
    cout << "I am OK" << endl;
}
};

struct AA
{
void front()
{
    cout << "AA front" << endl;
}
void back()
{
    cout << "AA back" << endl;
}
};

struct BB
{
void front()
{
    cout << "BB front" << endl;
}
};

struct CC
{
void back()
{
    cout << "CC back" << endl;
}
};

int main()
{

    IoCcontainer ioc;
    hashmap<string, int> hmap;
    cout << hmap.size() << endl;
  /* 
    invoke_aspect(AA(),BB(),CC(),DD())([](AA a,BB b, CC c, DD d){cout << "AA: " << &a << endl;cout << "BB: " << &b << endl;cout << "CC: " << &c << endl;cout << "DD: " << &d << endl;});
    //cout << *(obja.m_spw_obj) << endl;
    //cout << *(objb.m_spw_obj) << endl;
*/
    return 0;
}
