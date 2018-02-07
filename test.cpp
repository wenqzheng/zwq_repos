#include "utility/aspect.hpp"
#include "utility/thread_pool.hpp"
#include "utility/shared_ptr_wrapper.hpp"
#include <iostream>
#include <typeinfo>
#include <unistd.h>
#include <utility>
#include <string>
#include <optional>
#include <functional>


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

void GT()
{
    cout << "GT function" << endl;
}

void wraper()
{
    return GT();
}

int main()
{
    auto lmp([]{
        cout << "I am lambda" << endl;    
    });

    shared_ptr_wrapper<DD> sp_D = std::make_shared<DD>();
    DD dd;
    //cout << typeid(decltype(&(*sp_D)::front)).name() << endl;
    //auto f = function_wrapper(std::bind(&DD::front,&dd));
    thread_pool thrp;
    //thrp.submit(lmp);
    thrp.submit([&]{invoke_aspect<AA,DD>(std::bind(&DD::front,sp_D,std::placeholders::_1),5);});

    std::optional<string> str("zwq");
    cout << typeid(decltype(str)).name() << endl;
    wraper();
    //thrp.submit([&]{invoke<BB,CC>(GT);});
    sleep(2);
    return 0;
}
