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

class treenode
{
struct nodeconcept
{
    virtual ~nodeconcept() {}
    virtual bool isLeaf() const = 0;
};

template<typename T>
struct nodemodel:nodeconcept
{
    nodemodel(const T& t):
        node(t)
    {}

    virtual ~nodemodel() {}

    virtual bool isLeaf() const
    {
        return node.isLeaf();
    }

private:
    T node;
};

shared_ptr_wrapper<nodeconcept> __node;

public:
    treenode():
        __node(nullptr)
    {}

    template<typename T>
    treenode(const T& node):
        __node(make_shared<nodemodel<T>>(node))
    {}

    template<typename T>
    treenode& operator=(const T& node)
    {
        __node = make_shared<nodemodel<T>>(node);
        return *this;
    }

    inline bool isLeaf() const
    {
        return __node->isLeaf();
    }

};



struct AA
{
bool isLeaf() const
{
    return true;
}
};

struct BB
{
    AA aa;
bool isLeaf() const
{
    return false;
}
};
/*
template<typename T>
struct std::less<T>
{
    bool operator()(const T&, const AA&)
    {
        return true;
    }
};
*/
#define __HASH_TYPE(type) __hash_for_typeid(#type)

#define __str(type) #type

template<typename T>
struct __Info2Type
{
    using type = T;
    const size_t __idx;

    constexpr __Info2Type():
        __idx(__)
    {}

    __Info2Type(const __Info2Type& __info):
        __idx(__info.__idx)
    {}

};

template<typename T>
__Info2Type<T> __getType(const __Info2Type<T>& __info)
{
    return __Info2Type<T>();
}
/*
struct __object
{
    template<typename T>
    T operator()(const T& __t)
    {
    
    }
}
*/
#define __str2type(t) ( Type2Str<decltype(t)>(typeid(decltype(t)).hash_code()) )


int main()
{
    cout << sizeof(__Info2Type<int>) << endl;
    treenode node;
    int bb = 88;
    auto aa = __access_once(bb);
    type_index __idx = typeid(int);
    cout << (*reinterpret_cast<type_info**>(&__idx))->name() << endl;

    if (typeid(int) == typeid(bool)) cout << "OK" << endl; else cout << "NOT OK" << endl;
const char* ppp;
    cout << typeid(decltype(ppp)).name() << endl;
//    Type2Str<int> cc(88);
  //  decltype(__str2type(AA()))::Type cc;
    //cout << typeid(decltype(cc)).name() << endl;
   // cout << std::less<int>()(88, AA()) << endl;
}
