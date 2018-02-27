// shared_ptr_wrapper.hpp
// ----by wenqzheng
// HOOK MACRO __GTHREADS IN SHARED_PTR_ATOMIC.H FOR LOCK-FREE STD::SHARED_PTR.
// WRAP THIS LOCK-FREE STD::SHARED_PTR FOR MULTI-THREADS.
//-----------------------------------------------------------------------------

#pragma once

#include "../utility.hpp"
#include <memory>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <functional>

template<typename>
class shared_ptr_wrapper;

template<typename>
class weak_ptr_wrapper;

template<typename U>
struct std::hash<shared_ptr_wrapper<U>>
{
    auto operator()(const shared_ptr_wrapper<U>& sp_U)
    {
        return std::hash<std::shared_ptr<U>>()(sp_U.m_sp_ptr);
    }
};

template<typename U>
struct std::less<shared_ptr_wrapper<U>>
{
    auto operator()(const shared_ptr_wrapper<U>& sp_U1,
        const shared_ptr_wrapper<U>& sp_U2)
    {
        return std::less<std::shared_ptr<U>>()(
            sp_U1.m_sp_ptr, sp_U2.m_sp_ptr);
    }
};

template<typename U>
struct std::less_equal<shared_ptr_wrapper<U>>
{
    auto operator()(const shared_ptr_wrapper<U>& sp_U1,
        const shared_ptr_wrapper<U>& sp_U2)
    {
        return std::less_equal<std::shared_ptr<U>>()(
            sp_U1.m_sp_ptr, sp_U2.m_sp_ptr);
    }
};

template<typename U>
struct std::greater<shared_ptr_wrapper<U>>
{
    auto operator()(const shared_ptr_wrapper<U>& sp_U1,
        const shared_ptr_wrapper<U>& sp_U2)
    {
        return std::greater<std::shared_ptr<U>>()(
            sp_U1.m_sp_ptr, sp_U2.m_sp_ptr);
    }
};

template<typename U>
struct std::greater_equal<shared_ptr_wrapper<U>>
{
    auto operator()(const shared_ptr_wrapper<U>& sp_U1,
        const shared_ptr_wrapper<U>& sp_U2)
    {
        return std::greater_equal<std::shared_ptr<U>>()(
            sp_U1.m_sp_ptr, sp_U2.m_sp_ptr);
    }
};


template<typename T>
class alignas(__power2(sizeof(std::shared_ptr<void>))) shared_ptr_wrapper
{
private:
    std::shared_ptr<T> m_sp_ptr;
    
    template<typename>
    friend class std::hash;

    template<typename>
    friend class std::less;
    template<typename>
    friend class std::less_equal;
    template<typename>
    friend class std::greater;
    template<typename>
    friend class std::greater_equal;

    template<typename>
    friend class weak_ptr_wrapper;

public:
    shared_ptr_wrapper():
        m_sp_ptr(std::make_shared<T>())
    {}

    shared_ptr_wrapper(const shared_ptr_wrapper& sp):
        m_sp_ptr(std::atomic_load(&(sp.m_sp_ptr)))
    {}

    shared_ptr_wrapper& operator=(const shared_ptr_wrapper& sp)
    {
        std::atomic_store(&m_sp_ptr, sp.m_sp_ptr);
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper(const shared_ptr_wrapper<U>& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<const std::shared_ptr<T>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper(shared_ptr_wrapper<U>&& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper& operator=(const shared_ptr_wrapper<U>& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<const std::shared_ptr<T>*>(&sp));
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper& operator=(shared_ptr_wrapper<U>&& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<T>*>(&sp));
        return *this;
    }

    shared_ptr_wrapper(const std::shared_ptr<T>& sp):
        m_sp_ptr(std::atomic_load(&sp))
    {}

    shared_ptr_wrapper& operator=(const std::shared_ptr<T>& sp)
    {
        std::atomic_store(&m_sp_ptr, sp);
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper(const std::shared_ptr<U>& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<const std::shared_ptr<T>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper(std::shared_ptr<U>&& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper& operator=(const std::shared_ptr<U>& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<const std::shared_ptr<T>*>(&sp));
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper& operator=(std::shared_ptr<U>&& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<T>*>(&sp));
        return *this;
    }

    shared_ptr_wrapper(T* t):
        m_sp_ptr(std::shared_ptr<T>(t))
    {}

    shared_ptr_wrapper& operator=(T* t)
    {
        std::atomic_store(&m_sp_ptr, std::shared_ptr<T>(t));
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper(U* u):
        m_sp_ptr(std::shared_ptr<T>(reinterpret_cast<T*>(u)))
    {}

    template<typename U>
    shared_ptr_wrapper& operator=(U* u)
    {
        std::atomic_store(&m_sp_ptr,
            std::shared_ptr<T>(reinterpret_cast<T*>(u)));
        return *this;
    }

    shared_ptr_wrapper(std::nullptr_t):
        m_sp_ptr(nullptr)
    {}

    shared_ptr_wrapper& operator=(std::nullptr_t)
    {
        m_sp_ptr = nullptr;
        return *this;
    }

    T& operator*() const
    {
        return *m_sp_ptr;
    }

    T* operator->() const
    {
        return m_sp_ptr.get();
    }

    T* get() const
    {
        return m_sp_ptr.get();
    }

    explicit operator bool() const noexcept
    {
        return m_sp_ptr.get() != nullptr;
    }

    shared_ptr_wrapper load(std::memory_order order = std::memory_order_seq_cst)
    {
        return std::atomic_load_explicit(&m_sp_ptr, order);
    }

    template<typename U>
    void store(shared_ptr_wrapper<U> sp_other,
        std::memory_order order = std::memory_order_seq_cst)
    {
        std::atomic_store_explicit(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<T>*>(&sp_other), order);
    }

    template<typename U>
    shared_ptr_wrapper exchange(shared_ptr_wrapper<U> sp_W,
        std::memory_order order = std::memory_order_seq_cst)
    {
        return std::atomic_exchange(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<T>*>(&sp_W), order);
    }

    template<typename U1, typename U2>
    bool cas_weak(shared_ptr_wrapper<U1>& expected,
        shared_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
        return std::atomic_compare_exchange_weak_explicit(&m_sp_ptr,
            reinterpret_cast<std::shared_ptr<T>*>(&expected),
            *reinterpret_cast<std::shared_ptr<T>*>(&desired),
            success, failure);
    }

    template<typename U1, typename U2>
    bool cas_strong(shared_ptr_wrapper<U1>& expected,
        shared_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
        return std::atomic_compare_exchange_strong_explicit(&m_sp_ptr,
            reinterpret_cast<std::shared_ptr<T>*>(&expected),
            *reinterpret_cast<std::shared_ptr<T>*>(&desired),
            success, failure);
    }

    template<typename U>
    shared_ptr_wrapper<U> convert()
    {
        return *reinterpret_cast<std::shared_ptr<U>*>(&m_sp_ptr);
    }
};


template<>
class alignas(__power2(sizeof(std::shared_ptr<void>))) shared_ptr_wrapper<void>
{
private:
    std::shared_ptr<void> m_sp_ptr;

    template<typename>
    friend class std::hash;

    template<typename>
    friend class std::less;
    template<typename>
    friend class std::less_equal;
    template<typename>
    friend class std::greater;
    template<typename>
    friend class std::greater_equal;

    template<typename>
    friend class weak_ptr_wrapper;

public:
    shared_ptr_wrapper():
        m_sp_ptr(std::shared_ptr<void>())
    {}

    shared_ptr_wrapper(const shared_ptr_wrapper& sp):
        m_sp_ptr(std::atomic_load(&(sp.m_sp_ptr)))
    {}

    shared_ptr_wrapper& operator=(const shared_ptr_wrapper& sp)
    {
        std::atomic_store(&m_sp_ptr, sp.m_sp_ptr);
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper(const shared_ptr_wrapper<U>& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<const std::shared_ptr<void>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper(shared_ptr_wrapper<U>&& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<std::shared_ptr<void>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper& operator=(const shared_ptr_wrapper<U>& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<const std::shared_ptr<void>*>(&sp));
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper& operator=(shared_ptr_wrapper<U>&& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<void>*>(&sp));
        return *this;
    }

    shared_ptr_wrapper(const std::shared_ptr<void>& sp):
        m_sp_ptr(std::atomic_load(&sp))
    {}

    shared_ptr_wrapper& operator=(const std::shared_ptr<void>& sp)
    {
        std::atomic_store(&m_sp_ptr, sp);
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper(const std::shared_ptr<U>& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<const std::shared_ptr<void>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper(std::shared_ptr<U>&& sp):
        m_sp_ptr(std::atomic_load(
            reinterpret_cast<std::shared_ptr<void>*>(&sp)))
    {}

    template<typename U>
    shared_ptr_wrapper& operator=(const std::shared_ptr<U>& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<const std::shared_ptr<void>*>(&sp));
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper& operator=(std::shared_ptr<U>&& sp)
    {
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<void>*>(&sp));
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper(U* u):
        m_sp_ptr(reinterpret_cast<void*>(u))
    {}

    template<typename U>
    shared_ptr_wrapper& operator=(U* u)
    {
        std::atomic_store(&m_sp_ptr,
            std::shared_ptr<void>(reinterpret_cast<void*>(u)));
        return *this;
    }

    shared_ptr_wrapper(std::nullptr_t):
        m_sp_ptr(nullptr)
    {}

    shared_ptr_wrapper& operator=(std::nullptr_t)
    {
        m_sp_ptr = nullptr;
        return *this;
    }

    void* operator->() const
    {
        return reinterpret_cast<void*>(m_sp_ptr.get());
    }

    void* get() const
    {
        return reinterpret_cast<void*>(m_sp_ptr.get());
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    shared_ptr_wrapper load(std::memory_order order = std::memory_order_seq_cst)
    {
        return std::atomic_load_explicit(&m_sp_ptr, order);
    }
    
    template<typename U>
    void store(shared_ptr_wrapper<U> sp_other,
        std::memory_order order = std::memory_order_seq_cst)
    {
        std::atomic_store_explicit(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<void>*>(&sp_other), order);
    }

    template<typename U>
    shared_ptr_wrapper exchange(shared_ptr_wrapper<U> sp_W,
        std::memory_order order = std::memory_order_seq_cst)
    {
        return std::atomic_exchange_explicit(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<void>*>(&sp_W), order);
    }

    template<typename U1, typename U2>
    bool cas_weak(shared_ptr_wrapper<U1>& expected,
        shared_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
        return std::atomic_compare_exchange_weak_explicit(&m_sp_ptr,
            reinterpret_cast<std::shared_ptr<void>*>(&expected),
            *reinterpret_cast<std::shared_ptr<void>*>(&desired),
            success, failure);
    }

    template<typename U1, typename U2>
    bool cas_strong(shared_ptr_wrapper<U1>& expected,
        shared_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
        return std::atomic_compare_exchange_strong_explicit(&m_sp_ptr,
            reinterpret_cast<std::shared_ptr<void>*>(&expected),
            *reinterpret_cast<std::shared_ptr<void>*>(&desired),
            success, failure);
    }
    
    template<typename U>
    shared_ptr_wrapper<U> convert()
    {
        return *reinterpret_cast<std::shared_ptr<U>*>(&m_sp_ptr);
    }
};



template<typename U>
struct std::less<weak_ptr_wrapper<U>>
{
    auto operator()(const weak_ptr_wrapper<U>& wp_U1,
        const weak_ptr_wrapper<U>& wp_U2)
    {
        return std::less<std::weak_ptr<U>>()(
            wp_U1.m_wp_ptr, wp_U2.m_wp_ptr);
    }
};

template<typename U>
struct std::less_equal<weak_ptr_wrapper<U>>
{
    auto operator()(const weak_ptr_wrapper<U>& wp_U1,
        const weak_ptr_wrapper<U>& wp_U2)
    {
        return std::less_equal<std::weak_ptr<U>>()(
            wp_U1.m_wp_ptr, wp_U2.m_wp_ptr);
    }
};

template<typename U>
struct std::greater<weak_ptr_wrapper<U>>
{
    auto operator()(const weak_ptr_wrapper<U>& wp_U1,
        const weak_ptr_wrapper<U>& wp_U2)
    {
        return std::greater<std::weak_ptr<U>>()(
            wp_U1.m_wp_ptr, wp_U2.m_wp_ptr);
    }
};

template<typename U>
struct std::greater_equal<weak_ptr_wrapper<U>>
{
    auto operator()(const weak_ptr_wrapper<U>& wp_U1,
        const weak_ptr_wrapper<U>& wp_U2)
    {
        return std::greater_equal<std::weak_ptr<U>>()(
            wp_U1.m_wp_ptr, wp_U2.m_wp_ptr);
    }
};


template<typename T>
class alignas(__power2(sizeof(std::weak_ptr<void>))) weak_ptr_wrapper
{
private:
    std::weak_ptr<T> m_wp_ptr;

    template<typename>
    friend class std::less;
    template<typename>
    friend class std::less_equal;
    template<typename>
    friend class std::greater;
    template<typename>
    friend class std::greater_equal;

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
        m_wp_ptr = wp.m_wp_ptr;
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
        m_wp_ptr(*reinterpret_cast<const std::weak_ptr<T>*>(&wp))
    {}

    template<typename U>
    weak_ptr_wrapper(std::weak_ptr<U>&& wp):
        m_wp_ptr(*reinterpret_cast<std::weak_ptr<T>*>(&wp))
    {}

    template<typename U>
    weak_ptr_wrapper& operator=(const std::weak_ptr<U>& wp)
    {
        m_wp_ptr = *reinterpret_cast<const std::weak_ptr<T>*>(&wp);
        return *this;
    }

    template<typename U>
    weak_ptr_wrapper& operator=(std::weak_ptr<U>&& wp)
    {
        m_wp_ptr = *reinterpret_cast<std::weak_ptr<T>*>(&wp);
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
        m_wp_ptr = std::weak_ptr<T>();
    }

    void swap(weak_ptr_wrapper& wp) noexcept
    {
        std::atomic_exchange(
            reinterpret_cast<std::shared_ptr<T>*>(&m_wp_ptr),
            std::atomic_exchange(
                reinterpret_cast<std::shared_ptr<T>*>(&(wp.m_wp_ptr)),
                *reinterpret_cast<std::shared_ptr<T>*>(&m_wp_ptr)));
    }

    explicit operator bool() const noexcept
    {
        return m_wp_ptr.lock().get() != nullptr;
    }

    weak_ptr_wrapper load(std::memory_order order = std::memory_order_seq_cst)
    {
        std::shared_ptr<T> __tmp_ptr = std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&m_wp_ptr));
        return *reinterpret_cast<std::weak_ptr<T>*>(__tmp_ptr);
    }

    template<typename U>
    void store(weak_ptr_wrapper<U> wp_other,
        std::memory_order order = std::memory_order_seq_cst)
    {
        std::atomic_store(
            reinterpret_cast<std::shared_ptr<T>*>(&m_wp_ptr),
            *reinterpret_cast<std::shared_ptr<T>*>(&(wp_other.m_wp_ptr)));
    }

    template<typename U>
    weak_ptr_wrapper exchange(weak_ptr_wrapper<U> wp_W,
        std::memory_order order = std::memory_order_seq_cst)
    {
        std::shared_ptr<T> __tmp_ptr = std::atomic_exchange(
            reinterpret_cast<std::shared_ptr<T>*>(&m_wp_ptr),
            *reinterpret_cast<std::shared_ptr<T>*>(&(wp_W.m_wp_ptr)));
        return *reinterpret_cast<std::weak_ptr<T>*>(&__tmp_ptr);
    }

    template<typename U1, typename U2>
    bool cas_weak(weak_ptr_wrapper<U1>& expected,
        weak_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
        return std::atomic_compare_exchange_weak_explicit(
            reinterpret_cast<std::shared_ptr<T>*>(&m_wp_ptr),
            reinterpret_cast<std::shared_ptr<T>*>(&(expected.m_wp_ptr)),
            *reinterpret_cast<std::shared_ptr<T>*>(&(desired.m_wp_ptr)),
            success,
            failure);
    }

    template<typename U1, typename U2>
    bool cas_strong(weak_ptr_wrapper<U1>& expected,
        weak_ptr_wrapper<U2> desired,
        std::memory_order success = std::memory_order_seq_cst,
        std::memory_order failure = std::memory_order_seq_cst)
    {
         return std::atomic_compare_exchange_strong_explicit(
            reinterpret_cast<std::shared_ptr<T>*>(&m_wp_ptr),
            reinterpret_cast<std::shared_ptr<T>*>(&(expected.m_wp_ptr)),
            *reinterpret_cast<std::shared_ptr<T>*>(&(desired.m_wp_ptr)),
            success,
            failure);
    }

    template<typename U>
    weak_ptr_wrapper<U> convert()
    {
        return reinterpret_cast<shared_ptr_wrapper<T>*>(&m_wp_ptr)->convert();
    }
};
