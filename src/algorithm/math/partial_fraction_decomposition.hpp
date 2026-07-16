#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "polynomial_division.hpp"

namespace math{

template<int MOD>
struct PartialFractionDecompositionResult{
    Polynomial<MOD> polynomial_part;
    std::vector<Polynomial<MOD>> numerators;
    std::vector<Polynomial<MOD>> denominator_factors;
};

namespace partial_fraction_internal{

template<int MOD>
Polynomial<MOD> subtract(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right
){
    Polynomial<MOD> result(std::max(left.size(), right.size()));
    for(std::size_t index = 0; index < left.size(); ++index){
        result[index] += left[index];
    }
    for(std::size_t index = 0; index < right.size(); ++index){
        result[index] -= right[index];
    }
    polynomial_internal::trim(result);
    return result;
}

template<int MOD>
Polynomial<MOD> inverse_mod(
    Polynomial<MOD> value,
    const Polynomial<MOD>& modulus
){
    value = polynomial_remainder<MOD>(std::move(value), modulus);
    Polynomial<MOD> old_remainder = modulus;
    Polynomial<MOD> remainder = std::move(value);
    Polynomial<MOD> old_coefficient;
    Polynomial<MOD> coefficient{Modint<MOD>(1)};

    while(!remainder.empty()){
        auto division = polynomial_divmod<MOD>(
            old_remainder, remainder
        );
        Polynomial<MOD> next_coefficient = subtract<MOD>(
            old_coefficient,
            polynomial_multiply<MOD>(division.quotient, coefficient)
        );
        old_remainder = std::move(remainder);
        remainder = std::move(division.remainder);
        old_coefficient = std::move(coefficient);
        coefficient = std::move(next_coefficient);
    }
    if(old_remainder.size() != 1)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: denominator factors are not coprime "
            "(partial_fraction_decomposition)."
        );
    }
    const Modint<MOD> scale = old_remainder[0].inv();
    for(Modint<MOD>& item: old_coefficient) item *= scale;
    return polynomial_remainder<MOD>(std::move(old_coefficient), modulus);
}

} // namespace partial_fraction_internal

template<int MOD>
PartialFractionDecompositionResult<MOD> partial_fraction_decomposition(
    Polynomial<MOD> numerator,
    std::vector<Polynomial<MOD>> denominator_factors
){
    for(Polynomial<MOD>& factor: denominator_factors){
        polynomial_internal::trim(factor);
        if(factor.size() <= 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: denominator factor must have positive degree "
                "(partial_fraction_decomposition)."
            );
        }
    }
    polynomial_internal::trim(numerator);
    Polynomial<MOD> denominator{Modint<MOD>(1)};
    for(const Polynomial<MOD>& factor: denominator_factors){
        denominator = polynomial_multiply<MOD>(denominator, factor);
    }
    auto whole_division = polynomial_divmod<MOD>(
        std::move(numerator), denominator
    );

    std::vector<Polynomial<MOD>> part_numerators;
    part_numerators.reserve(denominator_factors.size());
    for(const Polynomial<MOD>& factor: denominator_factors){
        auto complementary_division = polynomial_divmod<MOD>(
            denominator, factor
        );
        if(!complementary_division.remainder.empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: internal non-factor "
                "(partial_fraction_decomposition)."
            );
        }
        Polynomial<MOD> inverse = partial_fraction_internal::inverse_mod<MOD>(
            complementary_division.quotient, factor
        );
        Polynomial<MOD> local_remainder = polynomial_remainder<MOD>(
            whole_division.remainder, factor
        );
        part_numerators.push_back(polynomial_remainder<MOD>(
            polynomial_multiply<MOD>(local_remainder, inverse),
            factor
        ));
    }
    return {
        std::move(whole_division.quotient),
        std::move(part_numerators),
        std::move(denominator_factors)
    };
}

} // namespace math
