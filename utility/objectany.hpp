#include "shared_ptr_wrapper.hpp"
#include "function_wrapper.hpp"
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <any>

class objectany
{
    using keyType = shared_ptr_wrapper<std::string>;
    using funcType = function_wrapper<std::any>;
    std::unordered_map<keyType, funcType> keymap;
}
