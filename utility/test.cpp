#include "lockfree_hashmap.hpp"
#include <iostream>
#include <string>
using namespace std;

int main()
{
    lockfree_hashmap<string, int> hmap;
    hmap.insert("4",8);
    cout << hmap.size() << endl;
    hmap.insert("8",16);
    hmap.insert("16",32);
    hmap.remove("8");
    auto pa = hmap.find("4");
    auto pb = hmap.find("16");
    cout << "pa: (" << (*pa).first << ", " << (*pa).second << ")" << endl;
    cout << "pb: (" << (*pb).first << ", " << (*pb).second << ")" << endl;
       // pa.swap(pb);
    return 0;
}
