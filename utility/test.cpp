#include "shared_ptr_wrapper.hpp"
#include "hashmap.hpp"
#include "Object.hpp"
#include <iostream>
#include <string>
#include <utility>
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

int main()
{
    object obja(f,4);
    object objb(g,4);
    //cout << *(obja.m_spw_obj) << endl;
    //cout << *(objb.m_spw_obj) << endl;
    return 0;
}
