#pragma once

#include <type_traits>

template<auto op_func, auto e_func, class T = std::decay_t<decltype(e_func())>>
struct Monoid{
    using S = T;
    constexpr S op(S a, S b)const{
        return op_func(a,b);
    }
    constexpr S e()const{
        return e_func();
    }
};