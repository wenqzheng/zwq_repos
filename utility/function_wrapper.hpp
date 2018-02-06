// function_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <memory>

class function_wrapper
{
struct impl_base
{
    virtual void call() = 0;
    virtual ~impl_base() {}
};

template<typename Func>
struct impl_type:impl_base
{
    impl_type(Func&& func_):func(std::move(func_)) {}
    void call() {func();}
private:
    Func func;
};

std::unique_ptr<impl_base> impl;

public:
function_wrapper() = default;
function_wrapper(const function_wrapper&) = delete;
function_wrapper(function_wrapper&) = delete;
function_wrapper& operator=(const function_wrapper&) = delete;

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

void operator()()
{
    impl->call();
}
};

