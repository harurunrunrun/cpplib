#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_PELL_EQUATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_PELL_EQUATION_HPP_INCLUDED

#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>

#include "../../integer/big_integer.hpp"

namespace math{

struct PellEquationSolution{
    BigInteger x;
    BigInteger y;
};

inline std::optional<PellEquationSolution>
minimum_pell_equation_solution(long long nonsquare){
    if(nonsquare <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: nonpositive D "
            "(minimum_pell_equation_solution)."
        );
    }
    long long square_root = static_cast<long long>(
        std::sqrt(static_cast<long double>(nonsquare))
    );
    while(static_cast<__int128>(square_root + 1)
        * (square_root + 1) <= nonsquare){
        ++square_root;
    }
    while(static_cast<__int128>(square_root) * square_root > nonsquare){
        --square_root;
    }
    if(square_root * square_root == nonsquare) return std::nullopt;

    long long offset = 0;
    long long denominator = 1;
    long long coefficient = square_root;
    BigInteger previous_previous_x = 0;
    BigInteger previous_x = 1;
    BigInteger previous_previous_y = 1;
    BigInteger previous_y = 0;
    while(true){
        BigInteger x =
            BigInteger(coefficient) * previous_x + previous_previous_x;
        BigInteger y =
            BigInteger(coefficient) * previous_y + previous_previous_y;
        if(x * x - BigInteger(nonsquare) * y * y == BigInteger(1)){
            return PellEquationSolution{std::move(x), std::move(y)};
        }
        previous_previous_x = std::move(previous_x);
        previous_x = std::move(x);
        previous_previous_y = std::move(previous_y);
        previous_y = std::move(y);
        offset = denominator * coefficient - offset;
        const __int128 next_denominator =
            (static_cast<__int128>(nonsquare)
                - static_cast<__int128>(offset) * offset)
            / denominator;
        denominator = static_cast<long long>(next_denominator);
        coefficient = (square_root + offset) / denominator;
    }
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_PELL_EQUATION_HPP_INCLUDED
