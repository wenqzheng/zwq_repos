// utility.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <climits>
#include <cassert>
#include <cstddef>
#include <memory>

#if defined(powerpc) || defined(__powerpc__) || defined(__ppc__)
#    define __CACHE_LINE_SIZE 128
#else
#    define __CACHE_LINE_SIZE 64
#endif

#define __container_of(ptr,type,field)                          \
({                                                              \
     const decltype(((type*)NULL)->field)* __tmp_ptr = (ptr);   \
     (type*)((char*)__tmp_ptr - offsetof(type, field));         \
})

inline bool __likely(bool expr)
{
#ifdef __GNUC__
    return __builtin_expect(expr, true);
#else
    return expr;
#endif
}

inline bool __unlikely(bool expr)
{
#ifdef __GNUC__
    return __builtin_expect(expr, false);
#else
    return expr;
#endif
}

inline constexpr unsigned long __power2(const unsigned long& size)
{
    assert(size >= 0 && size <= ULONG_MAX);
    if (0 == size)
        return 0;
    else {
        for (std::size_t i = 1; i < 64; ++i)
            if (!((size - 1) >> i))
                return (1 << i);
    }

    return 0;
}

#define __access_once(x) (*(volatile decltype(x) *)&(x))
