// shared_ptr_wrapper.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <functional>                       // std::hash

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
    shared_ptr_wrapper() = default;

    shared_ptr_wrapper(const shared_ptr_wrapper& sp)
	    :m_sp_ptr(std::atomic_load(&(sp.m_sp_ptr)))
    {}

    shared_ptr_wrapper& operator=(const shared_ptr_wrapper& sp)
    {
        std::atomic_store(&m_sp_ptr, sp.m_sp_ptr);
	    return *this;
    }

    shared_ptr_wrapper(const std::shared_ptr<T>& sp)
	    :m_sp_ptr(std::atomic_load(&sp))
    {}

    shared_ptr_wrapper& operator=(const std::shared_ptr<T>& sp)
    {
        std::atomic_store(&m_sp_ptr, sp);
	    return *this;
    }
/*    
    template<typename>
    std::enable_if_t<!std::is_void_v<T>>
    shared_ptr_wrapper(const T& t)
        :shared_ptr_wrapper(std::make_shared<T>(t))
    {}

    template<typename>
    std::enable_if_t<!std::is_void_v<T>>
    shared_ptr_wrapper(T&& t)
        :shared_ptr_wrapper(std::make_shared<T>(std::forward<T>(t)))
    {}

    template<typename>
    std::enable_if_t<!std::is_void_v<T>, std::reference_wrapper<T>>
    operator*() const
    {
        return *m_sp_ptr;
    }
*/
    T* operator->() const
    {
        return m_sp_ptr.get();
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
