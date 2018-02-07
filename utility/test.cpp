#include "aspect.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <typeinfo>
using namespace std;

struct AA
{
    void front()
    {
        cout << "front from AA" << endl;
    }

    void back()
    {
        cout << "back from AA" << endl;
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
    void front()
    {
        cout << "front from DD" << endl;
    }

    void back()
    {
        cout << "back from DD" << endl;
    }
};

void GT()
{
    cout << "GT function" << endl;
}

int main()
{
    auto lmp([]{
        cout << "I am lambda" << endl;    
    });
    thread_pool thrp;
    thrp.submit(lmp);
    thrp.submit([&lmp]{invoke<AA,BB>(lmp);});
    invoke<BB,CC>(GT);
    return 0;
}
