#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_FACTORIZATION_FINITE_FIELD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_FACTORIZATION_FINITE_FIELD_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "polynomial_half_gcd.hpp"

namespace math{

template<int MOD>
struct FiniteFieldPolynomialFactor{
    Polynomial<MOD> polynomial;
    std::size_t multiplicity = 0;
};

template<int MOD>
struct FiniteFieldPolynomialFactorization{
    Modint<MOD> leading_coefficient = Modint<MOD>(0);
    std::vector<FiniteFieldPolynomialFactor<MOD>> factors;
};

namespace polynomial_factorization_finite_field_internal{

constexpr bool is_prime_modulus(const int value){
    if(value < 2) return false;
    if(value % 2 == 0) return value == 2;
    for(long long divisor = 3;
        divisor * divisor <= value; divisor += 2){
        if(value % divisor == 0) return false;
    }
    return true;
}

template<int MOD>
bool is_one(const Polynomial<MOD>& polynomial){
    return polynomial.size() == 1
        && polynomial[0] == Modint<MOD>(1);
}

template<int MOD>
Polynomial<MOD> make_monic(Polynomial<MOD> polynomial){
    polynomial_internal::trim(polynomial);
    if(polynomial.empty()) return {};
    const Modint<MOD> inverse = polynomial.back().inv();
    for(Modint<MOD>& coefficient: polynomial) coefficient *= inverse;
    return polynomial;
}

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
Polynomial<MOD> divide_exact(
    const Polynomial<MOD>& numerator,
    const Polynomial<MOD>& denominator
){
    PolynomialDivModResult<MOD> division = polynomial_divmod<MOD>(
        numerator, denominator
    );
    if(!division.remainder.empty())[[unlikely]]{
        throw std::logic_error("finite-field polynomial division was not exact");
    }
    return make_monic<MOD>(std::move(division.quotient));
}

template<int MOD>
Polynomial<MOD> multiply_mod(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right,
    const Polynomial<MOD>& modulus
){
    return polynomial_remainder<MOD>(
        polynomial_multiply<MOD>(left, right), modulus
    );
}

template<int MOD>
Polynomial<MOD> power_mod(
    Polynomial<MOD> base,
    std::uint64_t exponent,
    const Polynomial<MOD>& modulus
){
    base = polynomial_remainder<MOD>(std::move(base), modulus);
    Polynomial<MOD> result{Modint<MOD>(1)};
    while(exponent != 0){
        if((exponent & 1U) != 0){
            result = multiply_mod<MOD>(result, base, modulus);
        }
        exponent >>= 1U;
        if(exponent != 0){
            base = multiply_mod<MOD>(base, base, modulus);
        }
    }
    return result;
}

template<int MOD>
void square_free_decomposition(
    Polynomial<MOD> polynomial,
    const std::size_t multiplicity_scale,
    std::vector<std::pair<Polynomial<MOD>, std::size_t>>& result
){
    polynomial = make_monic<MOD>(std::move(polynomial));
    if(polynomial.size() <= 1) return;
    const Polynomial<MOD> derivative = polynomial_derivative<MOD>(polynomial);
    if(derivative.empty()){
        Polynomial<MOD> root((polynomial.size() - 1) / MOD + 1);
        for(std::size_t degree = 0; degree < polynomial.size(); ++degree){
            if(polynomial[degree] == Modint<MOD>(0)) continue;
            if(degree % MOD != 0)[[unlikely]]{
                throw std::logic_error("invalid zero-derivative polynomial");
            }
            root[degree / MOD] = polynomial[degree];
        }
        if(multiplicity_scale >
           std::numeric_limits<std::size_t>::max() /
               static_cast<std::size_t>(MOD))[[unlikely]]{
            throw std::overflow_error("polynomial factor multiplicity overflow");
        }
        square_free_decomposition<MOD>(
            std::move(root),
            multiplicity_scale * static_cast<std::size_t>(MOD),
            result
        );
        return;
    }

    Polynomial<MOD> repeated = polynomial_gcd_half<MOD>(
        polynomial, derivative
    );
    Polynomial<MOD> remaining = divide_exact<MOD>(polynomial, repeated);
    std::size_t multiplicity = 1;
    while(!is_one<MOD>(remaining)){
        Polynomial<MOD> shared = polynomial_gcd_half<MOD>(
            remaining, repeated
        );
        Polynomial<MOD> component = divide_exact<MOD>(remaining, shared);
        if(!is_one<MOD>(component)){
            if(multiplicity >
               std::numeric_limits<std::size_t>::max() /
                   multiplicity_scale)[[unlikely]]{
                throw std::overflow_error(
                    "polynomial factor multiplicity overflow"
                );
            }
            result.emplace_back(
                std::move(component), multiplicity * multiplicity_scale
            );
        }
        remaining = std::move(shared);
        repeated = divide_exact<MOD>(repeated, remaining);
        ++multiplicity;
    }
    if(!is_one<MOD>(repeated)){
        Polynomial<MOD> root((repeated.size() - 1) / MOD + 1);
        for(std::size_t degree = 0; degree < repeated.size(); ++degree){
            if(repeated[degree] == Modint<MOD>(0)) continue;
            if(degree % MOD != 0)[[unlikely]]{
                throw std::logic_error("invalid repeated p-th power");
            }
            root[degree / MOD] = repeated[degree];
        }
        if(multiplicity_scale >
           std::numeric_limits<std::size_t>::max() /
               static_cast<std::size_t>(MOD))[[unlikely]]{
            throw std::overflow_error("polynomial factor multiplicity overflow");
        }
        square_free_decomposition<MOD>(
            std::move(root),
            multiplicity_scale * static_cast<std::size_t>(MOD),
            result
        );
    }
}

template<int MOD>
std::vector<std::pair<Polynomial<MOD>, std::size_t>>
distinct_degree_factorization(Polynomial<MOD> polynomial){
    std::vector<std::pair<Polynomial<MOD>, std::size_t>> result;
    const Polynomial<MOD> variable{Modint<MOD>(0), Modint<MOD>(1)};
    Polynomial<MOD> frobenius = variable;
    std::size_t degree = 1;
    while(polynomial.size() > 1
          && degree <= (polynomial.size() - 1) / 2){
        frobenius = power_mod<MOD>(
            frobenius, static_cast<std::uint64_t>(MOD), polynomial
        );
        Polynomial<MOD> factor = polynomial_gcd_half<MOD>(
            polynomial, subtract<MOD>(frobenius, variable)
        );
        if(!is_one<MOD>(factor)){
            result.emplace_back(factor, degree);
            polynomial = divide_exact<MOD>(polynomial, factor);
            if(is_one<MOD>(polynomial)) break;
            frobenius = polynomial_remainder<MOD>(
                std::move(frobenius), polynomial
            );
        }
        ++degree;
    }
    if(!is_one<MOD>(polynomial)){
        result.emplace_back(
            polynomial, polynomial.size() - 1
        );
    }
    return result;
}

class DeterministicRandom{
    std::uint64_t state_ = 0x9e3779b97f4a7c15ULL;

public:
    std::uint64_t operator()(){
        state_ ^= state_ >> 12;
        state_ ^= state_ << 25;
        state_ ^= state_ >> 27;
        return state_ * 0x2545f4914f6cdd1dULL;
    }
};

template<int MOD>
Polynomial<MOD> random_polynomial(
    const std::size_t degree,
    DeterministicRandom& random
){
    Polynomial<MOD> result(degree);
    for(Modint<MOD>& coefficient: result){
        coefficient = Modint<MOD>(static_cast<long long>(
            random() % static_cast<std::uint64_t>(MOD)
        ));
    }
    polynomial_internal::trim(result);
    return result;
}

template<int MOD>
Polynomial<MOD> cantor_zassenhaus_splitter(
    const Polynomial<MOD>& random_value,
    const std::size_t factor_degree,
    const Polynomial<MOD>& modulus
){
    if constexpr(MOD == 2){
        Polynomial<MOD> power = random_value;
        Polynomial<MOD> trace;
        for(std::size_t index = 0; index < factor_degree; ++index){
            trace = polynomial_add<MOD>(trace, power);
            power = power_mod<MOD>(power, 2, modulus);
        }
        return trace;
    }else{
        Polynomial<MOD> power = random_value;
        Polynomial<MOD> result{Modint<MOD>(1)};
        const std::uint64_t digit = static_cast<std::uint64_t>(
            (MOD - 1) / 2
        );
        for(std::size_t index = 0; index < factor_degree; ++index){
            result = multiply_mod<MOD>(
                result, power_mod<MOD>(power, digit, modulus), modulus
            );
            power = power_mod<MOD>(
                power, static_cast<std::uint64_t>(MOD), modulus
            );
        }
        if(result.empty()) result.resize(1);
        result[0] -= Modint<MOD>(1);
        polynomial_internal::trim(result);
        return result;
    }
}

template<int MOD>
void equal_degree_factorization(
    Polynomial<MOD> polynomial,
    const std::size_t factor_degree,
    DeterministicRandom& random,
    std::vector<Polynomial<MOD>>& result
){
    if(polynomial.size() - 1 == factor_degree){
        result.push_back(std::move(polynomial));
        return;
    }
    while(true){
        const Polynomial<MOD> candidate = random_polynomial<MOD>(
            polynomial.size() - 1, random
        );
        Polynomial<MOD> divisor = polynomial_gcd_half<MOD>(
            polynomial,
            cantor_zassenhaus_splitter<MOD>(
                candidate, factor_degree, polynomial
            )
        );
        if(is_one<MOD>(divisor) || divisor == polynomial) continue;
        Polynomial<MOD> quotient = divide_exact<MOD>(polynomial, divisor);
        equal_degree_factorization<MOD>(
            std::move(divisor), factor_degree, random, result
        );
        equal_degree_factorization<MOD>(
            std::move(quotient), factor_degree, random, result
        );
        return;
    }
}

template<int MOD>
bool polynomial_less(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right
){
    if(left.size() != right.size()) return left.size() < right.size();
    for(std::size_t index = left.size(); index-- > 0;){
        if(left[index].val() != right[index].val()){
            return left[index].val() < right[index].val();
        }
    }
    return false;
}

}  // namespace polynomial_factorization_finite_field_internal

template<int MOD>
FiniteFieldPolynomialFactorization<MOD>
polynomial_factorization_finite_field(Polynomial<MOD> polynomial){
    static_assert(
        polynomial_factorization_finite_field_internal::is_prime_modulus(MOD),
        "finite-field polynomial factorization requires prime MOD"
    );
    polynomial_internal::trim(polynomial);
    if(polynomial.empty())[[unlikely]]{
        throw std::invalid_argument("zero polynomial has no finite factorization");
    }
    FiniteFieldPolynomialFactorization<MOD> result;
    result.leading_coefficient = polynomial.back();
    if(polynomial.size() == 1) return result;
    polynomial =
        polynomial_factorization_finite_field_internal::make_monic<MOD>(
            std::move(polynomial)
        );

    std::vector<std::pair<Polynomial<MOD>, std::size_t>> square_free;
    polynomial_factorization_finite_field_internal::
        square_free_decomposition<MOD>(polynomial, 1, square_free);
    polynomial_factorization_finite_field_internal::DeterministicRandom random;
    for(auto& [component, multiplicity]: square_free){
        for(auto& [degree_component, factor_degree]:
            polynomial_factorization_finite_field_internal::
                distinct_degree_factorization<MOD>(std::move(component))){
            std::vector<Polynomial<MOD>> irreducible;
            polynomial_factorization_finite_field_internal::
                equal_degree_factorization<MOD>(
                    std::move(degree_component), factor_degree,
                    random, irreducible
                );
            for(Polynomial<MOD>& factor: irreducible){
                result.factors.push_back({std::move(factor), multiplicity});
            }
        }
    }
    std::sort(
        result.factors.begin(), result.factors.end(),
        [](const auto& left, const auto& right){
            return polynomial_factorization_finite_field_internal::
                polynomial_less<MOD>(left.polynomial, right.polynomial);
        }
    );
    return result;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_FACTORIZATION_FINITE_FIELD_HPP_INCLUDED
