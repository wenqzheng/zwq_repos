#include <atomic>
#include <iostream>

int main()
{
    std::atomic<int> x;
    int oldVal = 5, newVal = 10;
    x.store(oldVal);
    x.compare_exchange_strong(oldVal, newVal);
    std::cout << x << std::endl;
}
