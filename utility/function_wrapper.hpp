// function_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <utility>
#include <functional>

template<typename valueType = void>
class function_wrapper
{
struct impl_base
{
    virtual valueType call() = 0;
    virtual ~impl_base() {}
};

template<typename Func>
struct impl_type:impl_base
{
    impl_type(Func&& func_):func(std::move(func_)) {}
    valueType call() {return func();}
private:
    Func func;
};

function_wrapper(const function_wrapper&) = delete;
function_wrapper(function_wrapper&) = delete;
function_wrapper& operator=(const function_wrapper&) = delete;

std::unique_ptr<impl_base> impl;

public:
function_wrapper() = default;

template<typename Func>
function_wrapper(Func&& func)
    :impl(std::make_unique<impl_type<Func>>(std::move(func)))
{}

function_wrapper(function_wrapper&& func_)
    :impl(std::move(func_.impl))
{}

function_wrapper& operator=(function_wrapper&& func_)
{
    impl = std::move(func_.impl);
    return *this;
}

auto operator()()
{
    return impl->call();
}
};
