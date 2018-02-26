// utility.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <climits>
#include <cassert>

#if defined(powerpc) || defined(__powerpc__) || defined(__ppc__)
#    define CACHE_LINE_SIZE 128
#else
#    define CACHE_LINE_SIZE 64
#endif

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

constexpr unsigned long __power2(const unsigned long& size)
{
    assert(size >= 0 && size <= ULONG_MAX);
    if (0 == size)
        return 0;
    else {
        for (auto i = 0; i < 63; ++i)
            if (!(size >> (i + 1)))
                return (1 << i);
    }

    return 0;
}


