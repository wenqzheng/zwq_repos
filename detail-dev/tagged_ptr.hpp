// tagged_ptr.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#if defined(__x86_64) || defined(__x86_64__) || \
    defined(__amd64) || defined(__amd64__) || \
    defined(_M_X64)
#    include "tagged_ptr_ptrcompression.hpp"
#else
#    include "tagged_ptr_dcas.hpp"
#endif
