#ifndef CPPLIB_SRC_ALGORITHM_MATH_MIN_OF_MOD_OF_LINEAR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_MIN_OF_MOD_OF_LINEAR_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace math{

using min_of_mod_i128 = __int128_t;

// Returns min((a * x + b) mod modulus) for 0 <= x < count.
inline long long min_of_mod_of_linear(
    long long count,
    long long modulus,
    long long coefficient,
    long long constant
){
    if(count <= 0 || modulus <= 0 || coefficient < 0 || modulus <= coefficient ||
       constant < 0 || modulus <= constant)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: invalid argument (min_of_mod_of_linear)."
        );
    }

    long long answer = modulus - 1;
    while(true){
        // Reversing x makes the coefficient at most half the modulus.  This
        // guarantees that the Euclidean descent below has logarithmic depth.
        if(coefficient > modulus - coefficient){
            constant = static_cast<long long>(
                (static_cast<min_of_mod_i128>(coefficient) * (count - 1) + constant) %
                modulus
            );
            coefficient = modulus - coefficient;
        }

        answer = std::min(answer, constant);
        if(coefficient == 0) return answer;

        const min_of_mod_i128 maximum =
            static_cast<min_of_mod_i128>(coefficient) * (count - 1) + constant;
        const long long quotient_count = static_cast<long long>(maximum / modulus);
        if(quotient_count == 0) return answer;

        // Immediately after crossing q * modulus, the remainder is
        // (constant - q * modulus) mod coefficient.  Thus all possible new
        // prefix minima form another instance with a smaller modulus.
        const long long old_modulus_mod_coefficient = modulus % coefficient;
        const long long next_coefficient =
            (coefficient - old_modulus_mod_coefficient) % coefficient;
        const long long next_constant = static_cast<long long>(
            (static_cast<min_of_mod_i128>(constant % coefficient) + coefficient -
             old_modulus_mod_coefficient) % coefficient
        );

        count = quotient_count;
        modulus = coefficient;
        coefficient = next_coefficient;
        constant = next_constant;
    }
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_MIN_OF_MOD_OF_LINEAR_HPP_INCLUDED
