// weak_ptr_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "shared_ptr_wrapper.hpp"
#include <memory>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <functional>

template<typename T>
class weak_ptr_wrapper
{
private:
    std::weak_ptr<T> m_wp_ptr;

public:
    constexpr weak_ptr_wrapper():
        m_wp_ptr()
    {}

    weak_ptr_wrapper(const weak_ptr_wrapper& wp):
        m_wp_ptr(wp.m_wp_ptr)
    {}

    weak_ptr_wrapper(weak_ptr_wrapper&& wp):
        m_wp_ptr(wp.m_wp_ptr)
    {}

    weak_ptr_wrapper& operator=(const weak_ptr_wrapper& wp)
    {
        m_wp_ptr = wp.m_wp_ptr;
        return *this;
    }

    weak_ptr_wrapper& operator=(weak_ptr_wrapper&& wp)
    {
        m_wp_ptr = std::atomic_load(&(wp.m_wp_ptr));
        return *this;
    }

    weak_ptr_wrapper(const shared_ptr_wrapper<T>& sp):
        m_wp_ptr(std::atomic_load(&(sp.m_sp_ptr)))
    {}

    weak_ptr_wrapper(shared_ptr_wrapper<T>&& sp):
        m_wp_ptr(std::atomic_load(&(sp.m_sp_ptr)))
    {}

    weak_ptr_wrapper& operator=(const shared_ptr_wrapper<T>& sp)
    {
        m_wp_ptr = std::atomic_load(&(sp.m_sp_ptr));
        return *this;
    }

    weak_ptr_wrapper& operator=(shared_ptr_wrapper<T>&& sp)
    {
        m_wp_ptr = std::atomic_load(&(sp.m_sp_ptr));
        return *this;
    }

    weak_ptr_wrapper(const std::weak_ptr<T>& wp):
        m_wp_ptr(wp)
    {}

    weak_ptr_wrapper(std::weak_ptr<T>&& wp):
        m_wp_ptr(wp)
    {}

    weak_ptr_wrapper& operator=(const std::weak_ptr<T>& wp)
    {
        m_wp_ptr = wp;
        return *this;
    }

    weak_ptr_wrapper& operator=(std::weak_ptr<T>&& wp)
    {
        m_wp_ptr = wp;
        return *this;
    }

    weak_ptr_wrapper(const std::shared_ptr<T>& sp):
        m_wp_ptr(std::atomic_load(&sp))
    {}

    weak_ptr_wrapper(std::shared_ptr<T>&& sp):
        m_wp_ptr(std::atomic_load(&sp))
    {}

    weak_ptr_wrapper& operator=(const std::shared_ptr<T>& sp)
    {
        m_wp_ptr = std::atomic_load(&sp);
        return *this;
    }

    weak_ptr_wrapper& operator=(std::shared_ptr<T>&& sp)
    {
        m_wp_ptr = std::atomic_load(&sp);
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper(const weak_ptr_wrapper<U>& wp):
        m_wp_ptr(*reinterpret_cast<const std::weak_ptr<T>*>(&(wp.m_wp_ptr)))
    {}

    template<typename U>
    weak_ptr_wrapper(weak_ptr_wrapper<U>&& wp):
        m_wp_ptr(*reinterpret_cast<std::weak_ptr<T>*>(&(wp.m_wp_ptr)))
    {}

    template<typename U>
    weak_ptr_wrapper& operator=(const weak_ptr_wrapper<U>& wp)
    {
        m_wp_ptr = *reinterpret_cast<const std::weak_ptr<T>*>(&(wp.m_wp_ptr));
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper& operator=(weak_ptr_wrapper<U>&& wp)
    {
        m_wp_ptr = *reinterpret_cast<std::weak_ptr<T>*>(&(wp.m_wp_ptr));
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper(const shared_ptr_wrapper<U>& sp):
        m_wp_ptr(std::atomic_load(
            reinterpret_cast<const std::shared_ptr<T>*>(&(sp.m_sp_ptr))))
    {}

    template<typename U>
    weak_ptr_wrapper(shared_ptr_wrapper<U>&& sp):
        m_wp_ptr(std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&(sp.m_sp_ptr))))
    {}

    template<typename U>
    weak_ptr_wrapper& operator=(const shared_ptr_wrapper<U>& sp)
    {
        m_wp_ptr = std::atomic_load(
            reinterpret_cast<const std::shared_ptr<T>*>(&(sp.m_sp_ptr)));
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper& operator=(shared_ptr_wrapper<U>&& sp)
    {
        m_wp_ptr = std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&(sp.m_sp_ptr)));
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper(const std::weak_ptr<U>& wp):
        m_wp_ptr(reinterpret_cast<const std::weak_ptr<T>*>(&wp))
    {}

    template<typename U>
    weak_ptr_wrapper(std::weak_ptr<U>&& wp):
        m_wp_ptr(reinterpret_cast<std::weak_ptr<T>*>(&wp))
    {}

    template<typename U>
    weak_ptr_wrapper& operator=(const std::weak_ptr<U>& wp)
    {
        m_wp_ptr = reinterpret_cast<const std::weak_ptr<T>*>(&wp);
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper& operator=(std::weak_ptr<U>&& wp)
    {
        m_wp_ptr = reinterpret_cast<std::weak_ptr<T>*>(&wp);
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper(const std::shared_ptr<U>& sp):
        m_wp_ptr(std::atomic_load(
            reinterpret_cast<const std::shared_ptr<T>*>(&sp)))
    {}

    template<typename U>
    weak_ptr_wrapper(std::shared_ptr<U>&& sp):
        m_wp_ptr(std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&sp)))
    {}

    template<typename U>
    weak_ptr_wrapper& operator=(const std::shared_ptr<U>& sp)
    {
        m_wp_ptr = std::atomic_load(
            reinterpret_cast<const std::shared_ptr<T>*>(&sp));
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper& operator=(std::shared_ptr<U>&& sp)
    {
        m_wp_ptr = std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&sp));
        return *this;
    }

    weak_ptr_wrapper(std::nullptr_t):
        m_wp_ptr(nullptr)
    {}

    weak_ptr_wrapper& operator=(std::nullptr_t)
    {
        m_wp_ptr = nullptr;
        return *this;
    }

    std::size_t use_count() const noexcept
    {
        return m_wp_ptr.use_count();
    }

    bool expired() const noexcept
    {
        return m_wp_ptr.expired();
    }

    shared_ptr_wrapper<T> lock() const noexcept
    {
        return m_wp_ptr.lock();
    }

    bool owner_before() const noexcept
    {
        return m_wp_ptr.owner_before();
    }

    void reset() noexcept
    {
        std::weak_ptr<T> __tmp_wptr;
        std::shared_ptr<T> __tmp_sptr = __tmp_wptr.lock();
        m_wp_ptr = std::atomic_load(&__tmp_sptr);
    }

    void swap(weak_ptr_wrapper& wp) noexcept
    {
        wp.lock().exchange(lock());
    }

    explicit operator bool() const noexcept
    {
        return m_wp_ptr.lock().get() != nullptr;
    }

    weak_ptr_wrapper load(std::memory_order order = std::memory_order_seq_cst)
    {
        return lock().load(order);
    }

    template<typename U>
    void store(weak_ptr_wrapper<U> wp_other,
        std::memory_order order = std::memory_order_seq_cst)
    {
        lock().store(wp_other.lock(), order);
    }

    template<typename U>
    weak_ptr_wrapper exchange(weak_ptr_wrapper<U> wp_W,
        std::memory_order order = std::memory_order_seq_cst)
    {
        return lock().exchange(wp_W.lock(), order);
    }

    template<typename U1, typename U2>
    bool cas_weak(weak_ptr_wrapper<U1>& expected,
        weak_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
        return lock().cas_weak(
            expected.lock(),
            desired.lock(),
            success,
            failure);
    }

    template<typename U1, typename U2>
    bool cas_strong(weak_ptr_wrapper<U1>& expected,
        weak_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
        return lock().cas_strong(
            expected.lock(),
            desired.lock(),
            success,
            failure);
    }

    template<typename U>
    weak_ptr_wrapper<U> convert()
    {
        return lock().convert();
    }
};
