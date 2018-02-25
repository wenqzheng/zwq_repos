// tagged_pointer.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------)

#pragma once

#include "accelerator.hpp"
#include <cstddef>
#include <limits>
#include <cstdint>

template <class T>
class alignas(sizeof(void*)) tagged_ptr
{
    typedef std::uint64_t compressed_ptr_t;

public:
    typedef std::uint16_t tag_t;

protected:
    compressed_ptr_t ptr;

private:
    union cast_unit
    {
        compressed_ptr_t value;
        tag_t tag[4];
    };

    static const int tag_index = 3;
    static const compressed_ptr_t ptr_mask = 0xffffffffffffUL;

    static T* extract_ptr(volatile const compressed_ptr_t& i)
    {
        return (T*)(i & ptr_mask);
    }

    static tag_t extract_tag(volatile const compressed_ptr_t& i)
    {
        cast_unit cu;
        cu.value = i;
        return cu.tag[tag_index];
    }

    static compressed_ptr_t pack_ptr(T* ptr, tag_t tag)
    {
        cast_unit ret;
        ret.value = compressed_ptr_t(ptr);
        ret.tag[tag_index] = tag;
        return ret.value;
    }

public:
    tagged_ptr() noexcept = default;

    tagged_ptr(const tagged_ptr& p) = default;

    explicit tagged_ptr(T* p, tag_t t = 0):
        ptr(pack_ptr(p, t))
    {}

    tagged_ptr& operator=(const tagged_ptr&) = default;

    void set(T* p, tag_t t)
    {
        ptr = pack_ptr(p, t);
    }
    
    bool operator==(volatile const tagged_ptr& p) const
    {
        return (ptr == p.ptr);
    }

    bool operator!= (volatile const tagged_ptr& p) const
    {
        return !operator==(p);
    }
    
    T* get_ptr() const
    {
        return extract_ptr(ptr);
    }

    void set_ptr(T* p)
    {
        tag_t tag = get_tag();
        ptr = pack_ptr(p, tag);
    }
    
    tag_t get_tag() const
    {
        return extract_tag(ptr);
    }

    tag_t get_next_tag() const
    {
        tag_t next = (get_tag() + 1u) & (std::numeric_limits<tag_t>::max)();
        return next;
    }

    void set_tag(tag_t t)
    {
        T* p = get_ptr();
        ptr = pack_ptr(p, t);
    }
    
    T& operator*() const
    {
        return *get_ptr();
    }

    T* operator->() const
    {
        return get_ptr();
    }

    operator bool() const
    {
        return get_ptr() != 0;
    }
};
