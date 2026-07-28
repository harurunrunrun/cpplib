#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_NUMERIC_EXPANSION_ARITHMETIC_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_NUMERIC_EXPANSION_ARITHMETIC_DETAIL_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../../type/definition/geometry3d_expansion.hpp"
#include "../../aggregate/all/is_finite.hpp"

namespace geometry3d_expansion_detail{

inline void validate(long double value){
    if(!geometry3d_is_finite(value))[[unlikely]]{
        throw std::invalid_argument("non-finite expansion component");
    }
}

inline void validate(const Geometry3DExpansion& expansion){
    for(long double value: expansion) validate(value);
}

inline Geometry3DExpansion canonical(long double low, long double high){
    Geometry3DExpansion result;
    if(low != 0.0L) result.push_back(low);
    if(high != 0.0L) result.push_back(high);
    if(result.empty()) result.push_back(0.0L);
    return result;
}

inline Geometry3DExpansion grow(
    const Geometry3DExpansion& expansion,
    long double value
){
    validate(expansion);
    validate(value);
    Geometry3DExpansion result;
    result.reserve(expansion.size() + 1);
    long double accumulator = value;
    for(long double component: expansion){
        const long double sum = accumulator + component;
        if(!geometry3d_is_finite(sum))[[unlikely]]{
            throw std::overflow_error("expansion sum overflow");
        }
        const long double virtual_component = sum - accumulator;
        const long double error =
            (accumulator - (sum - virtual_component))
            + (component - virtual_component);
        if(error != 0.0L) result.push_back(error);
        accumulator = sum;
    }
    if(accumulator != 0.0L || result.empty()) result.push_back(accumulator);
    return result;
}

inline Geometry3DExpansion two_sum(long double first, long double second){
    validate(first);
    validate(second);
    const long double sum = first + second;
    if(!geometry3d_is_finite(sum))[[unlikely]]{
        throw std::overflow_error("two-sum overflow");
    }
    const long double virtual_second = sum - first;
    const long double error =
        (first - (sum - virtual_second)) + (second - virtual_second);
    return canonical(error, sum);
}

inline Geometry3DExpansion two_diff(long double first, long double second){
    return two_sum(first, -second);
}

inline Geometry3DExpansion two_product(long double first, long double second){
    validate(first);
    validate(second);
    const long double product = first * second;
    if(!geometry3d_is_finite(product))[[unlikely]]{
        throw std::overflow_error("two-product overflow");
    }
    const long double error = std::fma(first, second, -product);
    if(!geometry3d_is_finite(error))[[unlikely]]{
        throw std::overflow_error("two-product residual overflow");
    }
    return canonical(error, product);
}

inline Geometry3DExpansion sum(
    const Geometry3DExpansion& first,
    const Geometry3DExpansion& second
){
    validate(first);
    validate(second);
    Geometry3DExpansion result = first.empty()
        ? Geometry3DExpansion{0.0L} : first;
    for(long double component: second) result = grow(result, component);
    return result;
}

inline Geometry3DExpansion negate(Geometry3DExpansion expansion){
    validate(expansion);
    for(long double& component: expansion) component = -component;
    if(expansion.empty()) expansion.push_back(0.0L);
    return expansion;
}

inline Geometry3DExpansion scale(
    const Geometry3DExpansion& expansion,
    long double scalar
){
    validate(expansion);
    validate(scalar);
    Geometry3DExpansion result{0.0L};
    for(long double component: expansion){
        result = sum(result, two_product(component, scalar));
    }
    return result;
}

inline Geometry3DExpansion product(
    const Geometry3DExpansion& first,
    const Geometry3DExpansion& second
){
    validate(first);
    validate(second);
    Geometry3DExpansion result{0.0L};
    for(long double component: second){
        result = sum(result, scale(first, component));
    }
    return result;
}

inline long double estimate(const Geometry3DExpansion& expansion){
    validate(expansion);
    long double result = 0.0L;
    for(long double component: expansion) result += component;
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error("expansion estimate overflow");
    }
    return result;
}

inline int sign(const Geometry3DExpansion& expansion){
    validate(expansion);
    for(auto iterator = expansion.rbegin(); iterator != expansion.rend(); ++iterator){
        if(*iterator > 0.0L) return 1;
        if(*iterator < 0.0L) return -1;
    }
    return 0;
}

}  // namespace geometry3d_expansion_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_NUMERIC_EXPANSION_ARITHMETIC_DETAIL_HPP_INCLUDED
