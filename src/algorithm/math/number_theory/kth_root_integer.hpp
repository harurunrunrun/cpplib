#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_KTH_ROOT_INTEGER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_KTH_ROOT_INTEGER_HPP_INCLUDED

#include <bit>
#include <cstdint>
#include <stdexcept>

namespace math{
namespace kth_root_integer_internal{

inline bool power_less_equal(
    std::uint64_t base,
    unsigned int exponent,
    std::uint64_t limit
){
    if(base == 0) return true;
    using u128 = __uint128_t;
    u128 result = 1;
    u128 power = base;
    bool power_exceeds_limit = false;
    while(exponent != 0){
        if((exponent & 1U) != 0){
            if(power_exceeds_limit) return false;
            result *= power;
            if(result > limit) return false;
        }
        exponent >>= 1U;
        if(exponent == 0) break;
        if(power_exceeds_limit) continue;
        power *= power;
        if(power > limit) power_exceeds_limit = true;
    }
    return true;
}

} // namespace kth_root_integer_internal

inline std::uint64_t kth_root_integer(std::uint64_t value, unsigned int exponent){
    if(exponent == 0)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: zero exponent (kth_root_integer)."
        );
    }
    if(value <= 1 || exponent == 1) return value;
    if(exponent >= 64) return 1;

    const unsigned int value_bits = std::bit_width(value);
    const unsigned int root_bits = (value_bits + exponent - 1) / exponent;
    std::uint64_t lower = 0;
    std::uint64_t upper = std::uint64_t(1) << root_bits;
    while(lower + 1 < upper){
        const std::uint64_t middle = lower + (upper - lower) / 2;
        if(kth_root_integer_internal::power_less_equal(middle, exponent, value)){
            lower = middle;
        }else{
            upper = middle;
        }
    }
    return lower;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_KTH_ROOT_INTEGER_HPP_INCLUDED
