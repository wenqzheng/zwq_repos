// shared_ptr_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <functional>

template<typename T>
class shared_ptr_wrapper
{
private:
    std::shared_ptr<T> m_sp_ptr;

    std::size_t __sp_wrapper_hash() const
    {
        return std::hash<std::shared_ptr<T>>()(m_sp_ptr);
    }

    template<typename>
    friend class std::hash;

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

    shared_ptr_wrapper(const T& t):
        m_sp_ptr(std::make_shared<T>(t))
    {}

    shared_ptr_wrapper& operator=(const T& t)
    {
        std::atomic_store(&m_sp_ptr, std::make_shared<T>(t));
        return *this;
    }

    template<typename U>
    shared_ptr_wrapper(const U& u)
    {
        std::shared_ptr<U> __tmp = std::make_shared<U>(u);
        m_sp_ptr = std::atomic_load(
            reinterpret_cast<std::shared_ptr<T>*>(&__tmp));
    }

    template<typename U>
    shared_ptr_wrapper& operator=(const U& u)
    {
        std::shared_ptr<U> __tmp = std::make_shared<U>(u);
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<T>*>(&__tmp));
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
class shared_ptr_wrapper<void>
{
private:
    std::shared_ptr<void> m_sp_ptr;

    std::size_t __sp_wrapper_hash() const
    {
        return std::hash<std::shared_ptr<void>>()(m_sp_ptr);
    }

    template<typename>
    friend class std::hash;

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
    shared_ptr_wrapper(const U& u)
    {
        std::shared_ptr<U> __tmp = std::make_shared<U>(u);
        m_sp_ptr = std::atomic_load(
            reinterpret_cast<std::shared_ptr<void>*>(&__tmp));
    }

    template<typename U>
    shared_ptr_wrapper& operator=(const U& u)
    {
        std::shared_ptr<U> __tmp = std::make_shared<U>(u);
        std::atomic_store(&m_sp_ptr,
            *reinterpret_cast<std::shared_ptr<void>*>(&__tmp));
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
class std::hash<shared_ptr_wrapper<U>>
{
public:
    auto operator()(const shared_ptr_wrapper<U>& sp_U)
    {
        return sp_U.__sp_wrapper_hash();
    }
};
