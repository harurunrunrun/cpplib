#ifndef CPPLIB_SRC_STRUCTURE_TYPES_MONOID_ACT_LEN_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TYPES_MONOID_ACT_LEN_HPP_INCLUDED
#include <type_traits>

template<auto op_func, auto e_func, auto mapping_func, auto composition_func, auto id_func>
struct Monoid_Act_Len{
    using S = std::decay_t<decltype(e_func())>;
    using T = std::decay_t<decltype(id_func())>;
    
    constexpr S op(S a, long long len_a, S b, long long len_b)const{
        return op_func(a, len_a, b, len_b);
    }
    constexpr S e()const{
        return e_func();
    }
    constexpr S mapping(T f, S x, long long len)const{
        return mapping_func(f, x, len);
    }
    constexpr T composition(T f, T g)const{
        return composition_func(f, g);
    }
    constexpr T id()const{
        return id_func();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TYPES_MONOID_ACT_LEN_HPP_INCLUDED
