// ioc.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "shared_ptr_wrapper.hpp"
#include "function_wrapper.hpp"
#include "noncopyable.hpp"
#include <type_traits>
#include <functional>
#include <unordered_map>
#include <string>
#include <any>

class iocContainer:noncopyable
{
public:
    iocContainer() {} = default;
    !iocContainer() {}

    template<typename Type, typename Dep, typename... Args>
    std::enable_if_t<!std::is_base_of<Type, Dep>::value>
    registerType()
    {
        auto lmp([](Args... args) {
            shared_ptr_wrapper<Dep> depend = std::make_shared<Dep>(args...);
            shared_ptr_wrapper<Type> ret = std::make_shared<Type>(depend);
            return ret;
        });
        function_wrapper<shared_ptr_wrapper<Type>> func(
            std::bind(lmp, std::forward<Args>(args)...);
        registerType(key, func);
    }

    template<typename Type, typename Dep, typename... Args>
    std::enable_if_t<std::is_base_of<Type, Dep>::value>
    registerType(const std::string& key)
    {
        auto lmp([](Args... args) {
            shared_ptr_wrapper<Dep> ret = std::make_shared<Dep>(args);
            return ret;
        });
        function_wrapper<shared_ptr_wrapper<Type>> func =
            std::bind(lmp, Args...);
        registerType(key, func);
    }

    template<typename Type, typename... Args>
    void registerSingle(const std::string& key)
    {
        auto lmp([](Args... args) {
            shared_ptr_wrapper<Type> ret = std::make_shared<Type>(args);
            return ret;        
        });
        function_wrapper<shared_ptr_wrapper<Type>> func =
            std::bind(lmp, Args...);
        registerType(kep, func);
    }




}
