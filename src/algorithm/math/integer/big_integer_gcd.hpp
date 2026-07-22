#ifndef CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_GCD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_GCD_HPP_INCLUDED

#include <stdexcept>
#include <utility>

#include "big_integer.hpp"

struct BigIntegerExtendedGcdResult{
    BigInteger gcd;
    BigInteger x;
    BigInteger y;
};

inline BigInteger big_integer_gcd(BigInteger first, BigInteger second){
    first = first.absolute();
    second = second.absolute();
    while(!second.is_zero()){
        BigInteger remainder = BigInteger::divmod(first, second).second;
        first = std::move(second);
        second = std::move(remainder);
    }
    return first;
}

inline BigIntegerExtendedGcdResult big_integer_extended_gcd(
    BigInteger first,
    BigInteger second
){
    BigInteger old_remainder = std::move(first);
    BigInteger remainder = std::move(second);
    BigInteger old_x = 1;
    BigInteger x = 0;
    BigInteger old_y = 0;
    BigInteger y = 1;
    while(!remainder.is_zero()){
        auto [quotient, next_remainder] =
            BigInteger::divmod(old_remainder, remainder);
        old_remainder = std::move(remainder);
        remainder = std::move(next_remainder);

        BigInteger next_x = old_x - quotient * x;
        old_x = std::move(x);
        x = std::move(next_x);
        BigInteger next_y = old_y - quotient * y;
        old_y = std::move(y);
        y = std::move(next_y);
    }
    if(old_remainder.is_negative()){
        old_remainder = -old_remainder;
        old_x = -old_x;
        old_y = -old_y;
    }
    return {
        std::move(old_remainder),
        std::move(old_x),
        std::move(old_y)
    };
}

inline BigInteger big_integer_lcm(
    const BigInteger& first,
    const BigInteger& second
){
    if(first.is_zero() || second.is_zero()) return BigInteger(0);
    return abs((first / big_integer_gcd(first, second)) * second);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_INTEGER_BIG_INTEGER_GCD_HPP_INCLUDED
