
#include "utility/aspect_abstract.hpp"
#include "utility/thread_pool_block.hpp"
#include "utility/smart_ptr_wrapper.hpp"
#include "utility/function_wrapper.hpp"
#include "utility/hashmap_multiple.hpp"
#include <atomic>
#include <cstring>
#include <type_traits>
#include <iostream>
#include <typeinfo>
#include <unistd.h>
#include <utility>
#include <string>
#include <optional>
#include <functional>
#include <array>
#include <deque>
#include <unordered_map>
#include <any>
#include <variant>
#include <typeindex>
#include <string_view>
#include <algorithm>
#include <sys/smp.h>
#include <boost/fusion/container.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/hana.hpp>
using namespace std;

struct Fish {std::string name;};
struct Cat {uint64_t num;};
struct Dog {const char* aa;};

int main()
{
    auto animals = boost::hana::make_tuple(Fish{"Fish"},Cat{88},Dog{"Dog"});
    std::cout << typeid(decltype(animals)).name() << std::endl;
    std::cout << sizeof(animals) << std::endl;
    using namespace boost::hana::literals;
    auto gar1 = animals[0_c];
    auto gar2 = animals[1_c];
    std::cout << typeid(0_c).name() << std::endl;
    std::cout << sizeof(0_c) << std::endl;
    std::cout << typeid(1_c).name() << std::endl;
    std::cout << sizeof(1_c) << std::endl;
    std::cout << gar1.name << std::endl;
    std::cout << gar2.num << std::endl;
    auto animal_types = boost::hana::make_tuple(boost::hana::type_c<Fish>,boost::hana::type_c<Cat>,boost::hana::type_c<Dog>);
    std::cout << typeid(88).name() << std::endl;
    std::cout << typeid(boost::hana::tag_of_t<decltype(animal_types)>).name() << std::endl;
    std::cout << typeid(boost::hana::tag_of_t<Fish>).name() << std::endl;
    std::cout << typeid(boost::hana::tag_of_t<decltype(animals)>).name() << std::endl;
    std::cout << typeid(boost::hana::tag_of_t<decltype(animals[0_c])>).name() << std::endl;
    std::cout << typeid(boost::hana::tag_of_t<decltype(animals[1_c])>).name() << std::endl;
    std::cout << typeid(boost::hana::tag_of_t<decltype(2_c)>).name() << std::endl;
    std::cout << typeid(boost::hana::tag_of_t<decltype(3_c)>).name() << std::endl;
    if (std::is_same_v<decltype(0_c),decltype(4_c)>)
        cout << "the same" << endl;
    else
        cout << "not same" << endl;
    std::cout << sizeof(animal_types) << std::endl;

    auto lmd = [](int i)->std::string {
        return std::to_string(i*i);
    };

    auto has_name = boost::hana::is_valid([](auto&& x)->decltype((void)x.name){});
    auto has_num = boost::hana::is_valid([](auto&& x)->decltype((void)x.num){});
    auto has_aa = boost::hana::is_valid([](auto&& x)->decltype((void)x.aa){});
    std::vector<int> ints{1,2,3,4};
    std::vector<std::string> strings;
    std::transform(ints.begin(),ints.end(),std::back_inserter(strings),lmd);
    std::for_each(strings.begin(),strings.end(),[](auto& str){std::cout << str << std::endl;});

    cout << has_name(animals[0_c]) << endl;
    cout << has_num(animals[1_c]) << endl;
    cout << has_aa(animals[2_c]) << endl;

    auto llmd = boost::hana::is_valid([]{});
    cout << typeid(decltype(llmd)).name() << endl;
    cout << typeid(boost::hana::is_valid).name() << endl;
    return 0;
}


