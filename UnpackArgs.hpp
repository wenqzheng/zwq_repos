// UnpackArgs.hpp
// by wenqzheng
//-------------------------------------------------------------------------------
#pragma once

template<int N, int... Tail>
struct RCInt;
template<int N, int Tail>
struct RCInt<N, Tail>
{
    enum { value = Tail };
};
template<int N, int Head, int... Tail>
struct RCInt<N, Head, Tail...>
{
    enum { value = (N == sizeof...(Tail)) ? Head : RCInt<N, Tail...>::value };
};
template<int N, int... Ints>
struct UnpackInts
{
    enum { value = RCInt<(int)sizeof...(Ints) - N - 1, Ints...>::value };
};
