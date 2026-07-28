#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SCALAR_CHECKED_NONNEGATIVE_PRODUCT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SCALAR_CHECKED_NONNEGATIVE_PRODUCT_HPP_INCLUDED

#include <cmath>
#include <initializer_list>
#include <limits>
#include <stdexcept>

namespace geometry3d_detail{

inline long double checked_nonnegative_product(
    std::initializer_list<long double> factors,
    const char* overflow_message
){
    long double mantissa = 1.0L;
    long long exponent = 0;
    for(const long double factor: factors){
        if(!std::isfinite(factor) || factor < 0.0L)[[unlikely]]{
            throw std::invalid_argument(
                "product requires finite nonnegative factors"
            );
        }
        if(factor == 0.0L) return 0.0L;
        int factor_exponent = 0;
        const long double factor_mantissa =
            std::frexp(factor, &factor_exponent);
        mantissa *= factor_mantissa;
        exponent += factor_exponent;
        if(mantissa < 0.5L){
            mantissa *= 2.0L;
            --exponent;
        }
    }
    if(exponent > std::numeric_limits<long double>::max_exponent)
        [[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    const long double result = std::ldexp(
        mantissa, static_cast<int>(exponent)
    );
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    return result;
}

}  // namespace geometry3d_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SCALAR_CHECKED_NONNEGATIVE_PRODUCT_HPP_INCLUDED
