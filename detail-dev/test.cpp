#include "deque.hpp"
#include <atomic>
#include <string>
#include <iostream>

int main()
{
    deque<int> di;
    int a = 5;
    int b;
    di.push_left(a);
    std::cout << di.pop_left(&b) << std::endl;
}
