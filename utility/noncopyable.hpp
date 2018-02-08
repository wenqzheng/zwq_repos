// noncopyable.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

class noncopyable
{
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
