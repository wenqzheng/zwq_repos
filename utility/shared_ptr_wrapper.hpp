// shared_ptr_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <atomic>

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

    shared_ptr_wrapper(shared_ptr_wrapper&& sp)
	:m_sp_ptr(std::atomic_load(&(sp.m_sp_ptr)))
    {}

    shared_ptr_wrapper& operator=(const shared_ptr_wrapper& sp)
    {
        std::atomic_store(&m_sp_ptr, sp.m_sp_ptr);
	return *this;
    }

    shared_ptr_wrapper& operator=(shared_ptr_wrapper&& sp)
    {
        std::atomic_store(&m_sp_ptr, sp.m_sp_ptr);
	return *this;
    }

    shared_ptr_wrapper(const std::shared_ptr<T>& sp)
	:m_sp_ptr(std::atomic_load(&sp))
    {}

    shared_ptr_wrapper(std::shared_ptr<T>&& sp)
	:m_sp_ptr(std::atomic_load(&sp))
    {}

    shared_ptr_wrapper& operator=(const std::shared_ptr<T>& sp)
    {
        std::atomic_store(&m_sp_ptr, sp);
	return *this;
    }

    shared_ptr_wrapper& operator=(std::shared_ptr<T>&& sp)
    {
        std::atomic_store(&m_sp_ptr, sp);
	return *this;
    }

    T& operator*() const
    {
        return *m_sp_ptr;
    }

    T* operator->() const
    {
        return m_sp_ptr;
    }
};
