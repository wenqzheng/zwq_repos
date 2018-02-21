#include "shared_ptr_wrapper.hpp"
#include "hashmap.hpp"
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
    auto lmd = [=](auto&& f) {
        return [=](auto&&... args) {
            using ret_type = invoke_result_t<decltype(f),decltype(args)...>;
            shared_ptr_wrapper<ret_type> sp_R;
            return [=]() {
                sp_R = std::make_shared<ret_type>(f(args...));
            };
        };
    };

    auto lmd1 = [=]() {
        return lmd(g)(4);
    };

    auto lmd2 = [=]() {
        lmd1();
    };

    cout << typeid(decltype(lmd2)).name() << endl;


    return 0;
}
