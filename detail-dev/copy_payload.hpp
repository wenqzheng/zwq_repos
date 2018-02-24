// copy_loadable.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <type_traits>

struct copy_convertible
{
    template <typename T, typename U>
    static void copy(T & t, U & u)
    {
        u = t;
    }
};

struct copy_constructible_and_copyable
{
    template <typename T, typename U>
    static void copy(T & t, U & u)
    {
        u = U(t);
    }
};

template <typename T, typename U>
void copy_payload(T & t, U & u)
{
    typedef typename std::conditional_t<
        std::is_convertible_v<T, U>,
        copy_convertible,
        copy_constructible_and_copyable
    > copy_type;
    copy_type::copy(t, u);
}

template <typename T>
struct consume_via_copy
{
    consume_via_copy(T & out):
        out_(out)
    {}

    template <typename U>
    void operator()(U & element)
    {
        copy_payload(element, out_);
    }

    T & out_;
};

struct consume_noop
{
    template <typename U>
    void operator()(const U &)
    {
    }
};
