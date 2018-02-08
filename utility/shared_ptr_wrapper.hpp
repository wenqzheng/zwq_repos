// shared_ptr_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <atomic>

auto __any([](auto&& any) {return std::move(any);});
using __anyType = decltype(__any);

template<typename T>
class shared_ptr_wrapper
{
public:
    std::shared_ptr<T> m_sp_ptr;

public:
    shared_ptr_wrapper() = default;

    shared_ptr_wrapper(const shared_ptr_wrapper& sp)
	    :m_sp_ptr(std::atomic_load(&(sp.m_sp_ptr)))
    {}

    shared_ptr_wrapper& operator=(const shared_ptr_wrapper& sp)
// sp is thread-safe
    {
        std::atomic_store(&m_sp_ptr, sp.m_sp_ptr);
	return *this;
    }

    shared_ptr_wrapper(const std::shared_ptr<T>& sp)
	    :m_sp_ptr(std::atomic_load(&sp))
// const guarantee shared_ptr thread-safe
    {}

    shared_ptr_wrapper& operator=(const std::shared_ptr<T>& sp)
// const guarantee shared_ptr thread-safe
    {
        std::atomic_store(&m_sp_ptr, sp);
	return *this;
    }

    shared_ptr_wrapper(const T& t)
        :shared_ptr_wrapper(std::make_shared<T>(t))
// const guarantee T thread-safe
    {}

    shared_ptr_wrapper(T&& t)
        :shared_ptr_wrapper(std::make_shared<T>(std::forward<T>(t)))
// t is a rvalue is thread-safe
    {}

    T& operator*() const
// const guarantee thread-safe
    {
        return *m_sp_ptr;
    }

    T* operator->() const
// const guarantee thread-safe
    {
        return m_sp_ptr;
    }

};

