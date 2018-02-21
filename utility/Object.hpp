#include "shared_ptr_wrapper.hpp"
#include <memory>
#include <functional>
#include <type_traits>

class object
{
public:
    shared_ptr_wrapper<void> sp_V;
private:
    std::function<void()> func_vv;
public:
    template<typename funcType, typename... Args>
    object(funcType&& __func, Args&&... args)
    {
        auto lmd = [=](const auto& __func) {
            return [=](const auto&... args) {
                using retType = std::invoke_result_t<decltype(__func),
                    decltype(args)...>;
                return [=]() {
                    sp_V = shared_ptr_wrapper<retType>(
                        std::make_shared<retType>(__func(args...)));
                };
            };
        };
        func_vv = lmd(__func)(args...);
        func_vv();
    }
};
