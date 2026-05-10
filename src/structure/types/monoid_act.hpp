#pragma once
#include "monoid.hpp"

template<auto op_func, auto e_func, auto mapping_func, auto composition_func, auto id_func>
struct Monoid_Act: Monoid<op_func,e_func>{
    using S = std::decay_t<decltype(e_func())>;
    using T = std::decay_t<decltype(id_func())>;
    constexpr S mapping(T f, S x)const{
        return mapping_func(f, x);
    }
    constexpr T composition(T f, T g)const{
        return composition_func(f, g);
    }
    constexpr T id()const{
        return id_func();
    }
};