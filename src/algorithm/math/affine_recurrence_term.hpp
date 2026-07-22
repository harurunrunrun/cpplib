#ifndef CPPLIB_SRC_ALGORITHM_MATH_AFFINE_RECURRENCE_TERM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_AFFINE_RECURRENCE_TERM_HPP_INCLUDED

#include <cctype>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace math{

namespace affine_recurrence_internal{

struct Affine{
    std::uint64_t multiplier;
    std::uint64_t addition;
};

inline Affine compose(Affine outer, Affine inner, std::uint64_t modulus){
    return {
        static_cast<std::uint64_t>(
            static_cast<unsigned __int128>(outer.multiplier) * inner.multiplier %
            modulus
        ),
        static_cast<std::uint64_t>(
            (static_cast<unsigned __int128>(outer.multiplier) * inner.addition +
                outer.addition) % modulus
        )
    };
}

inline Affine power(Affine base, unsigned exponent, std::uint64_t modulus){
    Affine result{1 % modulus, 0};
    while(exponent != 0){
        if(exponent & 1) result = compose(result, base, modulus);
        exponent >>= 1;
        if(exponent != 0) base = compose(base, base, modulus);
    }
    return result;
}

inline std::uint64_t decimal_mod(
    const std::string& value,
    std::uint64_t modulus
){
    if(value.empty())[[unlikely]]{
        throw std::invalid_argument("empty decimal integer");
    }
    std::uint64_t result = 0;
    for(const unsigned char ch: value){
        if(!std::isdigit(ch))[[unlikely]]{
            throw std::invalid_argument("invalid decimal integer");
        }
        result = static_cast<std::uint64_t>(
            (static_cast<unsigned __int128>(result) * 10 + (ch - '0')) % modulus
        );
    }
    return result;
}

} // namespace affine_recurrence_internal

// F_0=1, F_n=a*F_(n-1)+b (mod modulus).
inline std::uint64_t affine_recurrence_term(
    const std::string& a,
    const std::string& b,
    const std::string& n,
    std::uint64_t modulus
){
    if(modulus == 0)[[unlikely]]{
        throw std::invalid_argument("affine_recurrence_term requires modulus > 0");
    }
    using namespace affine_recurrence_internal;
    const Affine transition{decimal_mod(a, modulus), decimal_mod(b, modulus)};
    Affine accumulated{1 % modulus, 0};
    if(n.empty())[[unlikely]] throw std::invalid_argument("empty decimal exponent");
    for(const unsigned char ch: n){
        if(!std::isdigit(ch))[[unlikely]]{
            throw std::invalid_argument("invalid decimal exponent");
        }
        accumulated = compose(
            power(accumulated, 10, modulus),
            power(transition, static_cast<unsigned>(ch - '0'), modulus),
            modulus
        );
    }
    return (accumulated.multiplier + accumulated.addition) % modulus;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_AFFINE_RECURRENCE_TERM_HPP_INCLUDED
