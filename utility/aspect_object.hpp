// aspect_object.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "shared_ptr_wrapper.hpp"
#include "function_wrapper.hpp"
#include <type_traits>
#include <utility>

#define HAS_MEMBER(member)                          \
template<typename T>                                \
struct has_member_##member                          \
{                                                   \
private:                                            \
    template<typename U>                            \
    static decltype(std::declval<U>().member(),     \
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

class aspect
{
    struct aspectConcept
    {
        virtual ~aspectConcept() {}
        virtual void front() = 0;
        virtual void back() = 0;
    };

    template<typename Asp, class enable = void>
    struct aspectModel:aspectConcept
    {
        aspectModel(const Asp& asp_)
            :asp(asp_)
        {}
        virtual ~aspectModel() {}
        virtual void front()
        {
            asp.front();
        }
        virtual void back()
        {
            asp.back();
        }

    private:
        Asp asp;
    };

    template<typename Asp,
             class enable = std::enable_if_t<
                 has_member_front<Asp>::value && 
                 !has_member_back<Asp>::value>
    struct aspectModel:aspectConcept
    {
        aspectModel(const Asp& asp_)
            :asp(asp_)
        {}
        virtual ~aspectModel() {}
        virtual void front() {asp.front();}
        virtual void back() {}

    private:
        Asp asp;
    };

    template<typename Asp,
             class enable = std::enable_if_t<
                 !has_member_front<Asp>::value && 
                 has_member_back<Asp>::value>
    struct aspectModel:aspectConcept
    {
        aspectModel(const Asp& asp_)
            :asp(asp_)
        {}
        virtual ~aspectModel() {}
        virtual void front() {}
        virtual void back() {asp.back();}

    private:
        Asp asp;
    };

    template<typename Asp,
             class enable = std::enable_if_t<
                 !has_member_front<Asp>::value && 
                 !has_member_back<Asp>::value>
    struct aspectModel:aspectConcept
    {
        aspectModel(const Asp& asp_)
            :asp(asp_)
        {}
        virtual ~aspectModel() {}
        virtual void front() {}
        virtual void back() {}

    private:
        Asp asp;
    };

    shared_ptr_wrapper<aspectConcept> asp;

public:
    template<typename Asp>
    aspect(const Asp& asp)
        :asp(std::make_shared<aspectModel<Asp>>(asp))
    {}

    void front() {asp->front();}
    void back() {asp->back();}
};

decltype(auto) invoke_aspect(aspect... aspList)
{
    return [=]<typename FuncType>(FuncType&& func) {
        (aspList.front(), ...);
        func(aspList, ...);
        (..., aspList.back());
    };
}
