// accelerator.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#if defined(powerpc) || defined(__powerpc__) || defined(__ppc__)
#    define CACHE_LINE_SIZE 128
#else
#    define CACHE_LINE_SIZE 64
#endif

inline bool likely(bool expr)
{
#ifdef __GNUC__
    return __builtin_expect(expr, true);
#else
    return expr;
#endif
}

inline bool unlikely(bool expr)
{
#ifdef __GNUC__
    return __builtin_expect(expr, false);
#else
    return expr;
#endif
}
