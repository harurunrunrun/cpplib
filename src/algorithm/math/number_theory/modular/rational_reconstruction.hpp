#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_RATIONAL_RECONSTRUCTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_RATIONAL_RECONSTRUCTION_HPP_INCLUDED

#include <cstdlib>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>

namespace math{

inline std::optional<std::pair<long long, long long>>
rational_reconstruction(
    long long residue,
    long long modulus,
    long long numerator_bound,
    long long denominator_bound
){
    if(modulus <= 0 || numerator_bound < 0 || denominator_bound <= 0)
        [[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid argument "
            "(rational_reconstruction)."
        );
    }
    residue %= modulus;
    if(residue < 0) residue += modulus;
    long long previous_remainder = modulus;
    long long remainder = residue;
    __int128 previous_coefficient = 0;
    __int128 coefficient = 1;
    while(remainder > numerator_bound){
        if(remainder == 0) return std::nullopt;
        const long long quotient = previous_remainder / remainder;
        const long long next_remainder =
            previous_remainder - quotient * remainder;
        previous_remainder = remainder;
        remainder = next_remainder;
        const __int128 next_coefficient =
            previous_coefficient
            - static_cast<__int128>(quotient) * coefficient;
        previous_coefficient = coefficient;
        coefficient = next_coefficient;
    }
    __int128 numerator = remainder;
    __int128 denominator = coefficient;
    if(denominator < 0){
        numerator = -numerator;
        denominator = -denominator;
    }
    if(denominator == 0 || denominator > denominator_bound
        || numerator < -numerator_bound || numerator > numerator_bound){
        return std::nullopt;
    }
    const long long result_numerator = static_cast<long long>(numerator);
    const long long result_denominator = static_cast<long long>(denominator);
    if(std::gcd(
        result_numerator < 0 ? -result_numerator : result_numerator,
        result_denominator
    ) != 1){
        return std::nullopt;
    }
    __int128 check =
        static_cast<__int128>(residue) * result_denominator
        - result_numerator;
    if(check % modulus != 0) return std::nullopt;
    return std::pair<long long, long long>{
        result_numerator, result_denominator
    };
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_RATIONAL_RECONSTRUCTION_HPP_INCLUDED
