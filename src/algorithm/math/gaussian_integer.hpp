#ifndef CPPLIB_SRC_ALGORITHM_MATH_GAUSSIAN_INTEGER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_GAUSSIAN_INTEGER_HPP_INCLUDED

#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace math{

struct GaussianInteger{
    long long real = 0;
    long long imag = 0;

    friend bool operator==(const GaussianInteger&, const GaussianInteger&) = default;
};

namespace gaussian_integer_internal{

using i128 = __int128_t;

inline long long narrow(i128 value){
    if(value < std::numeric_limits<long long>::min() ||
       value > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error("Gaussian integer component overflow");
    }
    return static_cast<long long>(value);
}

inline i128 norm(GaussianInteger value){
    const i128 real = value.real;
    const i128 imag = value.imag;
    return real * real + imag * imag;
}

inline i128 round_divide(i128 numerator, i128 denominator){
    if(numerator >= 0) return (numerator + denominator / 2) / denominator;
    return -((-numerator + denominator / 2) / denominator);
}

inline GaussianInteger remainder(GaussianInteger dividend, GaussianInteger divisor){
    const i128 denominator = norm(divisor);
    const i128 quotient_real = round_divide(
        static_cast<i128>(dividend.real) * divisor.real +
            static_cast<i128>(dividend.imag) * divisor.imag,
        denominator
    );
    const i128 quotient_imag = round_divide(
        static_cast<i128>(dividend.imag) * divisor.real -
            static_cast<i128>(dividend.real) * divisor.imag,
        denominator
    );
    const i128 remainder_real = static_cast<i128>(dividend.real) -
        (quotient_real * divisor.real - quotient_imag * divisor.imag);
    const i128 remainder_imag = static_cast<i128>(dividend.imag) -
        (quotient_real * divisor.imag + quotient_imag * divisor.real);
    return GaussianInteger{narrow(remainder_real), narrow(remainder_imag)};
}

inline GaussianInteger canonical_associate(GaussianInteger value){
    if(value.real == 0 && value.imag == 0) return value;
    const i128 real = value.real;
    const i128 imag = value.imag;
    const std::array<std::array<i128, 2>, 4> associates{{
        {real, imag},
        {-imag, real},
        {-real, -imag},
        {imag, -real},
    }};
    for(const auto& candidate: associates){
        if(candidate[0] > 0 && candidate[1] >= 0){
            return GaussianInteger{narrow(candidate[0]), narrow(candidate[1])};
        }
    }
    throw std::logic_error("library assertion fault: no canonical Gaussian associate.");
}

} // namespace gaussian_integer_internal

inline GaussianInteger gaussian_gcd(GaussianInteger left, GaussianInteger right){
    while(right.real != 0 || right.imag != 0){
        const GaussianInteger next = gaussian_integer_internal::remainder(left, right);
        left = right;
        right = next;
    }
    return gaussian_integer_internal::canonical_associate(left);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_GAUSSIAN_INTEGER_HPP_INCLUDED
