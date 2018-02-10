#include "lockfree_hashmap.hpp"
#include <iostream>
using namespace std;

int main()
{
    lockfree_hashmap<int, int> hmap;
    hmap.insert(4,8);
    cout << hmap.size() << endl;
    return 0;
}
