#include "utility/aspect_abstract.hpp"
#include "utility/thread_pool_block.hpp"
#include "utility/smart_ptr_wrapper.hpp"
#include "utility/function_wrapper.hpp"
#include "utility/hashmap_multiple.hpp"
#include <atomic>
#include <cstring>
#include <type_traits>
#include <iostream>
#include <typeinfo>
#include <unistd.h>
#include <utility>
#include <string>
#include <optional>
#include <functional>
#include <array>
#include <deque>
#include <unordered_map>
#include <any>
#include <variant>
#include <typeindex>
#include <string_view>
using namespace std;
/*
class __object
{
public:
    const char* __tyid;
    any __obj;
public:
    template<typename T>
    __object(const T& __t):
        __tyid(typeid(T).name()),
        __obj(any(__t))
    {}
};

typename<typename T>
struct typehash
{
};
*/

inline constexpr size_t __hash_for_typeid(const char* __typeid)
{
    size_t __hash = 0;
    for(size_t __ch= *__typeid; __ch; ++__ch)
        __hash = 131 * __hash + __ch;
    return __hash;
}

template<size_t __hash>
struct __type0
{};

template<typename T>
struct __type1
{
    using Type = T;
    __type0<__hash_for_typeid(typeid(T).name())> Type0;
};

/*
template<typename T>
constexpr std::size_t __typehash()
{
    return std::type_index(typeid(T)).hash_code();
}*/
/*
struct __type2
{
    using Type = T;
    std::size_t __hash
}

template<std::size_t __hash>
struct __type0
{
    using Type = 
};


struct __type1
{
    std:size_t __hash;
    __type1(const char* __typeid):
        __hash(std::hash<const char*>()(__typeid))
    {}
}



auto lmd = [](auto&& t) {
    return __type<decltype(t)>();
};
*/



int main()
{
  //  cout << typeid(decltype(lmd)).name() << endl;
  //  cout << sizeof(lmd) << endl;
  //  cout << __type<int>().__typeid << endl;
    std::array<int,hash<string_view>()("zwq")> arr;    

}

