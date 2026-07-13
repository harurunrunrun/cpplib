#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

#include "polynomial_interpolation.hpp"

namespace math{

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
