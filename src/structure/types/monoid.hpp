#ifndef CPPLIB_SRC_STRUCTURE_TYPES_MONOID_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TYPES_MONOID_HPP_INCLUDED

#include <type_traits>

template<auto op_func, auto e_func>
struct Monoid{
    using S = std::decay_t<decltype(e_func())>;
    constexpr S op(S a, S b)const{
        return op_func(a,b);
    }
    constexpr S e()const{
        return e_func();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TYPES_MONOID_HPP_INCLUDED
