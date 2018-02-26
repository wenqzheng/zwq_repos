// tagged_ptr.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

//#pragma once

#if defined(__x86_64) || defined(__x86_64__) || \
    defined(__amd64) || defined(__amd64__) || \
    defined(_M_X64)
#    include "tagged_ptr_ptrcompression.hpp"
#else
#    include "tagged_ptr_dcas.hpp"
#endif

#include <utility>
#include <atomic>
#include <iostream>
using namespace std;

int main()
{
    atomic<tagged_ptr<int>> atpint1;
    atomic<tagged_ptr<int>> atpint2;
    atomic<tagged_ptr<int>> a;
    atpint1.compare_exchange_strong(atpint2,a.get_ptr());
}
