// unordered_map_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "shared_ptr_wrapper.hpp"
#include <atomic>
#include <utility>
#include <unordered_map>

template<typename keyType, typename valueType>
class unordered_map_wrapper
{
    class joint
    {
    public:
        std::unordered_map<keyType, valueType> hmap;
        std::atomic_ulong readercounter;
        std::atomic_ulong writerversion;

        joint():
            :readercounter(0), writerversion(0)
        {}

        joint(const std::unordered_map<keyType, valueType>& _hmap)
            :hmap(_hmap), readercounter(0), writerversion(0)
        {}

        joint(const joint& jt)
            :hmap(jt.hashmap), readercounter(0), writerversion(0)
        {}
    };

    shared_ptr_wrapper<joint> hashmap;

public:
    using key_type = keyType;
    using mapped_type = valueType;
    using value_type = std::pair<const keyType, valueType>;

public:
    unordered_map_wrapper()
    {}

    unordered_map_wrapper(const std::unordered_map<keyType, valueType>& hmap)
        :hashmap(hmap)
    {}

    unordered_map_wrapper(const unordered_map_wrapper<keyType, valueType>& hmap)
        :hashmap(hmap.hashmap)
    {}

    bool insert(keyType&& _key, valueType&& _val)
    {
        if (hashmap->readercounter > 0)
    }


}
