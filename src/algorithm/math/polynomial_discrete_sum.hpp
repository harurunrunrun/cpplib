#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_DISCRETE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_DISCRETE_SUM_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../fft/convolution.hpp"
#include "polynomial_interpolation.hpp"

template<int mod>
struct FastModint;

namespace math{

namespace polynomial_discrete_sum_detail{

template<class Mint>
struct StaticModintTraits{
    static constexpr bool supported = false;
};

template<int MOD>
struct StaticModintTraits<::Modint<MOD>>{
    static constexpr bool supported = true;
    static constexpr int modulus = MOD;
};

template<int MOD>
struct StaticModintTraits<::FastModint<MOD>>{
    static constexpr bool supported = true;
    static constexpr int modulus = MOD;
};

template<int MOD>
std::vector<Modint<MOD>> inverse_series(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
){
    if(size == 0) return {};
    std::vector<Modint<MOD>> result{values.front().inv()};
    while(result.size() < size){
        const std::size_t next_size = std::min(size, result.size() * 2);
        std::vector<Modint<MOD>> prefix(
            values.begin(),
            values.begin() + static_cast<std::ptrdiff_t>(next_size)
        );
        std::vector<Modint<MOD>> correction =
            arbitrary_mod_convolution<MOD>(prefix, result);
        correction.resize(next_size);
        for(Modint<MOD>& value: correction) value = -value;
        correction.front() += Modint<MOD>(2);
        result = arbitrary_mod_convolution<MOD>(result, correction);
        result.resize(next_size);
    }
    return result;
}

}  // namespace polynomial_discrete_sum_detail

template<class Mint, std::size_t MAX_DEGREE>
struct PolynomialDiscreteSum{
    using CoefficientArray = std::array<Mint, MAX_DEGREE + 1>;
    using AntiderivativeArray = std::array<Mint, MAX_DEGREE + 2>;

private:
    polynomial_interpolation_detail::ConsecutiveLagrangeTable<
        Mint, MAX_DEGREE + 1
    > lagrange;

    static void check_exponent(std::size_t exponent){
        if(exponent > MAX_DEGREE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (Faulhaber power sum)."
            );
        }
    }

    static void check_coefficient_count(std::size_t coefficient_count){
        if(coefficient_count > MAX_DEGREE + 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (polynomial discrete sum)."
            );
        }
    }

    AntiderivativeArray discrete_antiderivative_quadratic(
        const CoefficientArray& coefficients,
        std::size_t coefficient_count
    ) const{
        AntiderivativeArray result{};
        for(std::size_t reverse = coefficient_count; reverse > 0; --reverse){
            const std::size_t degree = reverse - 1;
            Mint residual = coefficients[degree];
            for(std::size_t target_degree = degree + 2;
                target_degree <= coefficient_count;
                ++target_degree){
                residual -= result[target_degree]
                    * lagrange.combination(target_degree, degree);
            }
            result[degree + 1] = residual
                / Mint(static_cast<long long>(degree + 1));
        }
        return result;
    }

    template<int MOD>
    AntiderivativeArray discrete_antiderivative_fast(
        const CoefficientArray& coefficients,
        std::size_t coefficient_count
    ) const{
        using CanonicalMint = Modint<MOD>;
        if(coefficient_count == 0) return {};

        std::vector<CanonicalMint> denominator(coefficient_count);
        for(std::size_t index = 0; index < coefficient_count; ++index){
            denominator[index] = CanonicalMint(
                lagrange.inverse_factorial[index + 1].val()
            );
        }
        const std::vector<CanonicalMint> bernoulli_over_factorial =
            polynomial_discrete_sum_detail::inverse_series<MOD>(
                denominator, coefficient_count
            );

        std::vector<CanonicalMint> reversed_scaled(coefficient_count);
        for(std::size_t degree = 0; degree < coefficient_count; ++degree){
            reversed_scaled[coefficient_count - 1 - degree] =
                CanonicalMint(coefficients[degree].val())
                * CanonicalMint(lagrange.factorial[degree].val());
        }
        const std::vector<CanonicalMint> correlation =
            arbitrary_mod_convolution<MOD>(
                reversed_scaled, bernoulli_over_factorial
            );

        AntiderivativeArray result{};
        for(std::size_t degree = 0; degree < coefficient_count; ++degree){
            result[degree + 1] = Mint(
                correlation[coefficient_count - 1 - degree].val()
            ) * lagrange.inverse_factorial[degree + 1];
        }
        return result;
    }

public:
    Mint faulhaber(Mint n, std::size_t exponent) const{
        check_exponent(exponent);
        if(exponent == 0) return n;

        AntiderivativeArray values{};
        for(std::size_t index = 0; index <= exponent; ++index){
            values[index + 1] = values[index]
                + Mint(static_cast<long long>(index)).pow(
                    static_cast<long long>(exponent)
                );
        }
        return lagrange.evaluate(values, exponent + 2, n);
    }

    Mint power_sum(long long n, std::size_t exponent) const{
        if(n < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative length (power_sum)."
            );
        }
        return faulhaber(Mint(n), exponent);
    }

    AntiderivativeArray discrete_antiderivative(
        const CoefficientArray& coefficients,
        std::size_t coefficient_count
    ) const{
        check_coefficient_count(coefficient_count);
        using traits =
            polynomial_discrete_sum_detail::StaticModintTraits<Mint>;
        if constexpr(traits::supported){
            if constexpr(traits::modulus >= 2){
                return discrete_antiderivative_fast<traits::modulus>(
                    coefficients, coefficient_count
                );
            }
        }
        return discrete_antiderivative_quadratic(
            coefficients, coefficient_count
        );
    }

    Mint polynomial_prefix_sum(
        const CoefficientArray& coefficients,
        std::size_t coefficient_count,
        Mint n
    ) const{
        const AntiderivativeArray antiderivative = discrete_antiderivative(
            coefficients, coefficient_count
        );
        return polynomial_evaluate(antiderivative, coefficient_count + 1, n);
    }

    Mint polynomial_prefix_sum(
        const CoefficientArray& coefficients,
        std::size_t coefficient_count,
        long long n
    ) const{
        if(n < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative length (polynomial_prefix_sum)."
            );
        }
        return polynomial_prefix_sum(coefficients, coefficient_count, Mint(n));
    }
};

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_DISCRETE_SUM_HPP_INCLUDED
