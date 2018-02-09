#include "shared_ptr_wrapper.hpp"
#include <memory>
#include <iostream>
#include <functional>
#include <type_traits>

class object
{
    std::shared_ptr<void> p_tmp;
public:
    shared_ptr_wrapper<void> m_spw_obj;
    
    template<typename funcType, typename... Args>
    object(funcType&& _func, Args&&... _args)
    {
        auto lmd = [=](const auto& __func) {
            return [=](const auto&... __args) {
                using retType = std::invoke_result_t<decltype(__func),
                    decltype(__args)...>;
                return [=]() {
                    p_tmp = std::make_shared<retType>(__func(__args...));
		    m_spw_obj = std::reinterpret_pointer_cast<retType>(p_tmp);
		    std::cout << typeid(decltype(m_spw_obj)).name() << std::endl;
                };
            };
        };
        lmd(_func)(_args...)();
    }
};
