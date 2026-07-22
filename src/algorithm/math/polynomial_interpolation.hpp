#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_INTERPOLATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_INTERPOLATION_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>

#include "polynomial_interpolation_fast.hpp"

namespace math{

template<class T, std::size_t CAPACITY>
T polynomial_evaluate(
    const std::array<T, CAPACITY>& coefficients,
    std::size_t coefficient_count,
    T x
){
    if(coefficient_count > CAPACITY)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (polynomial_evaluate)."
        );
    }
    T result = T(0);
    for(std::size_t index = coefficient_count; index > 0; --index){
        result = result * x + coefficients[index - 1];
    }
    return result;
}

namespace polynomial_interpolation_detail{

template<class Mint, std::size_t MAX_DEGREE>
struct ConsecutiveLagrangeTable{
    std::array<Mint, MAX_DEGREE + 1> factorial{};
    std::array<Mint, MAX_DEGREE + 1> inverse_factorial{};

    ConsecutiveLagrangeTable(){
        factorial[0] = Mint(1);
        for(std::size_t index = 1; index <= MAX_DEGREE; ++index){
            factorial[index] = factorial[index - 1]
                * Mint(static_cast<long long>(index));
        }
        inverse_factorial[MAX_DEGREE] = factorial[MAX_DEGREE].inv();
        for(std::size_t index = MAX_DEGREE; index > 0; --index){
            inverse_factorial[index - 1] = inverse_factorial[index]
                * Mint(static_cast<long long>(index));
        }
    }

    Mint combination(std::size_t n, std::size_t k) const{
        if(n > MAX_DEGREE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (fixed polynomial combination)."
            );
        }
        if(k > n) return Mint(0);
        return factorial[n] * inverse_factorial[k] * inverse_factorial[n - k];
    }

    Mint evaluate(
        const std::array<Mint, MAX_DEGREE + 1>& values,
        std::size_t sample_count,
        Mint x
    ) const{
        if(sample_count == 0 || sample_count > MAX_DEGREE + 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (consecutive Lagrange evaluation)."
            );
        }

        std::array<Mint, MAX_DEGREE + 2> prefix{};
        std::array<Mint, MAX_DEGREE + 2> suffix{};
        prefix[0] = Mint(1);
        for(std::size_t index = 0; index < sample_count; ++index){
            prefix[index + 1] = prefix[index]
                * (x - Mint(static_cast<long long>(index)));
        }
        suffix[sample_count] = Mint(1);
        for(std::size_t index = sample_count; index > 0; --index){
            suffix[index - 1] = suffix[index]
                * (x - Mint(static_cast<long long>(index - 1)));
        }

        Mint result = Mint(0);
        for(std::size_t index = 0; index < sample_count; ++index){
            Mint term = values[index] * prefix[index] * suffix[index + 1]
                * inverse_factorial[index]
                * inverse_factorial[sample_count - 1 - index];
            if(((sample_count - 1 - index) & 1U) != 0) term = -term;
            result += term;
        }
        return result;
    }
};
template<class Mint>
struct StaticModintTraits{
    static constexpr bool supported = false;
};

template<int MOD>
struct StaticModintTraits<::Modint<MOD>>{
    static constexpr bool supported = true;
    static constexpr int modulus = MOD;
};


}  // namespace polynomial_interpolation_detail

template<class Mint, std::size_t MAX_DEGREE>
struct PolynomialInterpolation{
    using ValueArray = std::array<Mint, MAX_DEGREE + 1>;

private:
    polynomial_interpolation_detail::ConsecutiveLagrangeTable<
        Mint, MAX_DEGREE
    > consecutive;

    static void check_sample_count(std::size_t sample_count){
        if(sample_count == 0 || sample_count > MAX_DEGREE + 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (PolynomialInterpolation)."
            );
        }
    }

public:
    Mint evaluate_consecutive(
        const ValueArray& values,
        std::size_t sample_count,
        Mint x
    ) const{
        check_sample_count(sample_count);
        return consecutive.evaluate(values, sample_count, x);
    }

    Mint evaluate_equidistant(
        const ValueArray& values,
        std::size_t sample_count,
        Mint first,
        Mint step,
        Mint x
    ) const{
        check_sample_count(sample_count);
        if(step == Mint(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: zero step (equidistant Lagrange evaluation)."
            );
        }
        return consecutive.evaluate(values, sample_count, (x - first) / step);
    }

    ValueArray interpolate_coefficients(
        const ValueArray& xs,
        const ValueArray& ys,
        std::size_t sample_count
    ) const{
        check_sample_count(sample_count);
        using traits =
            polynomial_interpolation_detail::StaticModintTraits<Mint>;
        if constexpr(traits::supported){
            try{
                const auto end = static_cast<std::ptrdiff_t>(sample_count);
                const std::vector<Mint> points(xs.begin(), xs.begin() + end);
                const std::vector<Mint> values(ys.begin(), ys.begin() + end);
                const auto coefficients = polynomial_interpolation<
                    traits::modulus
                >(points, values);
                ValueArray result{};
                for(std::size_t index = 0;
                    index < coefficients.size();
                    ++index){
                    result[index] = coefficients[index];
                }
                return result;
            }catch(const std::invalid_argument& error){
                throw std::runtime_error(error.what());
            }
        }

        ValueArray denominators{};
        for(std::size_t index = 0; index < sample_count; ++index){
            denominators[index] = Mint(1);
            for(std::size_t other = 0; other < sample_count; ++other){
                if(index == other) continue;
                const Mint difference = xs[index] - xs[other];
                if(difference == Mint(0))[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: duplicate interpolation points."
                    );
                }
                denominators[index] *= difference;
            }
        }

        std::array<Mint, MAX_DEGREE + 2> prefix_products{};
        prefix_products[0] = Mint(1);
        for(std::size_t index = 0; index < sample_count; ++index){
            prefix_products[index + 1] = prefix_products[index] * denominators[index];
        }
        Mint inverse_product = prefix_products[sample_count].inv();
        for(std::size_t index = sample_count; index > 0; --index){
            const Mint denominator = denominators[index - 1];
            denominators[index - 1] = inverse_product * prefix_products[index - 1];
            inverse_product *= denominator;
        }

        std::array<Mint, MAX_DEGREE + 2> product{};
        product[0] = Mint(1);
        std::size_t product_degree = 0;
        for(std::size_t index = 0; index < sample_count; ++index){
            product[product_degree + 1] = product[product_degree];
            for(std::size_t degree = product_degree; degree > 0; --degree){
                product[degree] = product[degree - 1] - xs[index] * product[degree];
            }
            product[0] = -xs[index] * product[0];
            ++product_degree;
        }

        ValueArray result{};
        for(std::size_t index = 0; index < sample_count; ++index){
            const Mint scale = ys[index] * denominators[index];
            Mint quotient_coefficient = product[sample_count];
            result[sample_count - 1] += scale * quotient_coefficient;
            for(std::size_t degree = sample_count - 1; degree > 0; --degree){
                quotient_coefficient = product[degree]
                    + xs[index] * quotient_coefficient;
                result[degree - 1] += scale * quotient_coefficient;
            }
        }
        return result;
    }

    Mint evaluate(
        const ValueArray& xs,
        const ValueArray& ys,
        std::size_t sample_count,
        Mint x
    ) const{
        const ValueArray coefficients = interpolate_coefficients(xs, ys, sample_count);
        return polynomial_evaluate(coefficients, sample_count, x);
    }
};

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_INTERPOLATION_HPP_INCLUDED
