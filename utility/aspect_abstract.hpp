// aspect.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include <type_traits>
#include <utility>

#define HAS_MEMBER(member)                          \
template<typename T, typename... Args>              \
struct has_member_##member                          \
{                                                   \
private:                                            \
    template<typename U>                            \
    static decltype(std::declval<U>().member(       \
        std::declval<Args>()...),                   \
        std::true_type()) Check(int);               \
    template<typename U>                            \
    static std::false_type Check(...);              \
public:                                             \
    enum {                                          \
        value = std::is_same<decltype(Check<T>(0)), \
            std::true_type>::value                  \
    };                                              \
}

HAS_MEMBER(front);
HAS_MEMBER(back);

template<typename FuncType, typename... Args>
class aspect
{
    aspect(const aspect&) = delete;
    aspect(aspect&) = delete;
    aspect& operator=(const aspect&) = delete;
    FuncType m_func;

public:
    aspect() = default;
    aspect(FuncType&& func)
        :m_func(std::forward<FuncType>(func))
    {}

    template<typename Asp>
    std::enable_if_t<has_member_front<Asp, Args...>::value &&
        has_member_back<Asp, Args...>::value>
    invoke(Args&&... args, Asp&& asp)
    {
        asp.front(std::forward<Args>(args)...);
        m_func(std::forward<Args>(args)...);
        asp.back(std::forward<Args>(args)...);
    }

    template<typename Asp>
    std::enable_if_t<has_member_front<Asp, Args...>::value &&
        !has_member_back<Asp, Args...>::value>
    invoke(Args&&... args, Asp&& asp)
    {
        asp.front(std::forward<Args>(args)...);
        m_func(std::forward<Args>(args)...);
    }

    template<typename Asp>
    std::enable_if_t<!has_member_front<Asp, Args...>::value &&
        has_member_back<Asp, Args...>::value>
    invoke(Args&&... args, Asp&& asp)
    {
        m_func(std::forward<Args>(args)...);
        asp.back(std::forward<Args>(args)...);
    }

    template<typename AspHead, typename... AspTail>
    std::enable_if_t<has_member_front<AspHead, Args...>::value &&
        has_member_back<AspHead, Args...>::value>
    invoke(Args&&... args, AspHead&& asphead, AspTail&&... asptail)
    {
        asphead.front(std::forward<Args>(args)...);
        invoke<AspTail...>(std::forward<Args>(args)...,
            std::forward<AspTail>(asptail)...);
        asphead.back(std::forward<Args>(args)...);
    }

    template<typename AspHead, typename... AspTail>
    std::enable_if_t<has_member_front<AspHead, Args...>::value &&
        !has_member_back<AspHead, Args...>::value>
    invoke(Args&&... args, AspHead&& asphead, AspTail&&... asptail)
    {
        asphead.front(std::forward<Args>(args)...);
        invoke<AspTail...>(std::forward<Args>(args)...,
            std::forward<AspTail>(asptail)...);
    }

    template<typename AspHead, typename... AspTail>
    std::enable_if_t<!has_member_front<AspHead, Args...>::value &&
        has_member_back<AspHead, Args...>::value>
    invoke(Args&&... args, AspHead&& asphead, AspTail&&... asptail)
    {
        invoke<AspTail...>(std::forward<Args>(args)...,
            std::forward<AspTail>(asptail)...);
        asphead.back(std::forward<Args>(args)...);
    }

};

template<typename... Asp, typename... Args, typename FuncType>
void invoke_aspect(FuncType&& func, Args&&... args)
{
    aspect<FuncType, Args...> asp(std::forward<FuncType>(func));
    asp.invoke(std::forward<Args>(args)..., Asp()...);
}
