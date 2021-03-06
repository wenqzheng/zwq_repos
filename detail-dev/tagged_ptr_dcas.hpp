// tagged_ptr.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <limits>

template <class T>
class alignas(2 * sizeof(void*)) tagged_ptr
{
public:
    typedef std::size_t tag_t;

    tagged_ptr() noexcept = default;

    tagged_ptr(tagged_ptr const & p) = default;

    tagged_ptr& operator=(const tagged_ptr& p) = default;
    
    explicit tagged_ptr(T* p, tag_t t = 0):
        ptr(p),
        tag(t)
    {}

    void set(T* p, tag_t t)
    {
        ptr = p;
        tag = t;
    }
    
    bool operator==(volatile const tagged_ptr& p) const
    {
        return (ptr == p.ptr) && (tag == p.tag);
    }

    bool operator!=(volatile const tagged_ptr& p) const
    {
        return !operator==(p);
    }
    
    T* get_ptr() const
    {
        return ptr;
    }

    void set_ptr(T* p)
    {
        ptr = p;
    }
    
    tag_t get_tag() const
    {
        return tag;
    }

    tag_t get_next_tag() const
    {
        tag_t next = (get_tag() + 1) & (std::numeric_limits<tag_t>::max)();
        return next;
    }

    void set_tag(tag_t t)
    {
        tag = t;
    }
    
    T& operator*() const
    {
        return *ptr;
    }

    T* operator->() const
    {
        return ptr;
    }

    operator bool() const
    {
        return ptr != 0;
    }

protected:
    T* ptr;
    tag_t tag;
};

