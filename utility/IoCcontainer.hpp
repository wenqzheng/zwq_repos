// ioc.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "../utility.hpp"
#include "shared_ptr_wrapper.hpp"
#include "function_wrapper.hpp"
#include "hashmap_unique.hpp"
#include "noncopyable.hpp"
#include <type_traits>
#include <functional>
#include <string>
#include <any>

class IoCcontainer
{
    hashmap<std::string, std::any> m_regedit_map;

    bool registerCreator(const std::string& __strKey, std::any __conFunc)
    {
        if (m_regedit_map.find(__strKey) != m_regedit_map.end())
            return false;

        m_regedit_map.insert(__strKey, __conFunc);
        return true;
    }

public:
    IoCcontainer() {}
    ~IoCcontainer() {}

    template<typename Type, typename Dep, typename... Args>
    std::enable_if_t<!std::is_base_of_v<Type, Dep>>
    regeditRel(const std::string& __strKey)
    {
        using __funcType = std::function<shared_ptr_wrapper<Type>(Args...)>;
        __funcType __func = [](Args... args) {
            return shared_ptr_wrapper<Type>(std::make_shared<Type>(
                std::make_shared<Dep>(args...)
            ));
        };
        registerCreator(__strKey, __func);
    }

    template<typename Type, typename Dep, typename... Args>
    std::enable_if_t<std::is_base_of_v<Type, Dep>>
    regeditRel(const std::string& __strKey)
    {
        using __funcType = std::function<shared_ptr_wrapper<Type>(Args...)>;
        __funcType __func = [](Args... args) {
            return shared_ptr_wrapper<Type>(std::make_shared<Dep>(args...));
        };
        registerCreator(__strKey, __func);
    }

    template<typename Type, typename... Args>
    void regeditType(const std::string& __strKey)
    {
        using __funcType = std::function<shared_ptr_wrapper<Type>(Args...)>;
        __funcType __func = [](Args... args) {
            return shared_ptr_wrapper<Type>(std::make_shared<Type>(args...));
        };
        registerCreator(__strKey, __func);
    }

    template<typename Type, typename... Args>
    shared_ptr_wrapper<Type> resolve(const std::string& __strKey, Args... args)
    {
        auto iter = m_regedit_map.find(__strKey);
        if (iter == m_regedit_map.end())
            return nullptr;

        std::any anyRes = iter->second;
        using __funcType = std::function<shared_ptr_wrapper<Type>(Args...)>;
        __funcType __func = std::any_cast<__funcType>(anyRes);

        return __func(args...);
    }
};
