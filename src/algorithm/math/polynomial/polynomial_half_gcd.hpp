#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_HALF_GCD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_HALF_GCD_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "polynomial_division.hpp"

namespace math{

template<int MOD>
struct PolynomialMatrix2x2{
    Polynomial<MOD> top_left;
    Polynomial<MOD> top_right;
    Polynomial<MOD> bottom_left;
    Polynomial<MOD> bottom_right;
};

template<int MOD>
struct PolynomialExtendedGcdResult{
    Polynomial<MOD> gcd;
    Polynomial<MOD> left_coefficient;
    Polynomial<MOD> right_coefficient;
};

namespace polynomial_half_gcd_internal{

template<int MOD>
Polynomial<MOD> negate(Polynomial<MOD> value){
    for(Modint<MOD>& coefficient: value) coefficient = -coefficient;
    polynomial_internal::trim(value);
    return value;
}

template<int MOD>
Polynomial<MOD> high_part(
    const Polynomial<MOD>& value,
    const std::size_t skipped_coefficients
){
    if(skipped_coefficients >= value.size()) return {};
    return Polynomial<MOD>(
        value.begin() + static_cast<std::ptrdiff_t>(skipped_coefficients),
        value.end()
    );
}

template<int MOD>
PolynomialMatrix2x2<MOD> identity_matrix(){
    return {{Modint<MOD>(1)}, {}, {}, {Modint<MOD>(1)}};
}

template<int MOD>
PolynomialMatrix2x2<MOD> swap_matrix(){
    return {{}, {Modint<MOD>(1)}, {Modint<MOD>(1)}, {}};
}

template<int MOD>
PolynomialMatrix2x2<MOD> multiply_matrix(
    const PolynomialMatrix2x2<MOD>& left,
    const PolynomialMatrix2x2<MOD>& right
){
    return {
        polynomial_add<MOD>(
            polynomial_multiply<MOD>(left.top_left, right.top_left),
            polynomial_multiply<MOD>(left.top_right, right.bottom_left)
        ),
        polynomial_add<MOD>(
            polynomial_multiply<MOD>(left.top_left, right.top_right),
            polynomial_multiply<MOD>(left.top_right, right.bottom_right)
        ),
        polynomial_add<MOD>(
            polynomial_multiply<MOD>(left.bottom_left, right.top_left),
            polynomial_multiply<MOD>(left.bottom_right, right.bottom_left)
        ),
        polynomial_add<MOD>(
            polynomial_multiply<MOD>(left.bottom_left, right.top_right),
            polynomial_multiply<MOD>(left.bottom_right, right.bottom_right)
        )
    };
}

template<int MOD>
std::pair<Polynomial<MOD>, Polynomial<MOD>> apply_matrix(
    const PolynomialMatrix2x2<MOD>& matrix,
    const Polynomial<MOD>& first,
    const Polynomial<MOD>& second
){
    return {
        polynomial_add<MOD>(
            polynomial_multiply<MOD>(matrix.top_left, first),
            polynomial_multiply<MOD>(matrix.top_right, second)
        ),
        polynomial_add<MOD>(
            polynomial_multiply<MOD>(matrix.bottom_left, first),
            polynomial_multiply<MOD>(matrix.bottom_right, second)
        )
    };
}

template<int MOD>
PolynomialMatrix2x2<MOD> half_gcd_recursive(
    const Polynomial<MOD>& first,
    const Polynomial<MOD>& second
){
    const std::size_t target_degree = first.size() / 2;
    if(second.empty() || second.size() - 1 < target_degree){
        return identity_matrix<MOD>();
    }

    const PolynomialMatrix2x2<MOD> first_matrix =
        half_gcd_recursive<MOD>(
            high_part<MOD>(first, target_degree),
            high_part<MOD>(second, target_degree)
        );
    auto [reduced_first, reduced_second] = apply_matrix<MOD>(
        first_matrix, first, second
    );
    if(reduced_second.empty() ||
       reduced_second.size() - 1 < target_degree){
        return first_matrix;
    }

    PolynomialDivModResult<MOD> division = polynomial_divmod<MOD>(
        reduced_first, reduced_second
    );
    const std::size_t shift = 2 * target_degree -
        (reduced_second.size() - 1);
    const PolynomialMatrix2x2<MOD> euclidean_step{
        {}, {Modint<MOD>(1)},
        {Modint<MOD>(1)}, negate<MOD>(std::move(division.quotient))
    };
    const PolynomialMatrix2x2<MOD> second_matrix =
        half_gcd_recursive<MOD>(
            high_part<MOD>(reduced_second, shift),
            high_part<MOD>(division.remainder, shift)
        );
    return multiply_matrix<MOD>(
        multiply_matrix<MOD>(second_matrix, euclidean_step),
        first_matrix
    );
}

template<int MOD>
PolynomialMatrix2x2<MOD> gcd_matrix(
    const Polynomial<MOD>& first,
    const Polynomial<MOD>& second
){
    if(first.empty()) return swap_matrix<MOD>();
    if(second.empty()) return identity_matrix<MOD>();
    if(first.size() <= second.size()){
        PolynomialDivModResult<MOD> division = polynomial_divmod<MOD>(
            second, first
        );
        const PolynomialMatrix2x2<MOD> step{
            {Modint<MOD>(1)}, {},
            negate<MOD>(std::move(division.quotient)), {Modint<MOD>(1)}
        };
        return multiply_matrix<MOD>(
            gcd_matrix<MOD>(first, division.remainder), step
        );
    }
    const PolynomialMatrix2x2<MOD> reduction =
        half_gcd_recursive<MOD>(first, second);
    auto [reduced_first, reduced_second] = apply_matrix<MOD>(
        reduction, first, second
    );
    return multiply_matrix<MOD>(
        multiply_matrix<MOD>(
            gcd_matrix<MOD>(reduced_second, reduced_first),
            swap_matrix<MOD>()
        ),
        reduction
    );
}

} // namespace polynomial_half_gcd_internal

template<int MOD>
std::pair<Polynomial<MOD>, Polynomial<MOD>> apply_polynomial_matrix(
    const PolynomialMatrix2x2<MOD>& matrix,
    const Polynomial<MOD>& first,
    const Polynomial<MOD>& second
){
    return polynomial_half_gcd_internal::apply_matrix<MOD>(
        matrix, first, second
    );
}

template<int MOD>
PolynomialMatrix2x2<MOD> polynomial_half_gcd(
    Polynomial<MOD> first,
    Polynomial<MOD> second
){
    polynomial_internal::trim(first);
    polynomial_internal::trim(second);
    if(first.empty() ||
       (!second.empty() && first.size() <= second.size()))[[unlikely]]{
        throw std::invalid_argument(
            "polynomial_half_gcd requires deg(first) > deg(second)"
        );
    }
    return polynomial_half_gcd_internal::half_gcd_recursive<MOD>(
        first, second
    );
}

template<int MOD>
PolynomialExtendedGcdResult<MOD> polynomial_extended_gcd_half(
    Polynomial<MOD> first,
    Polynomial<MOD> second
){
    polynomial_internal::trim(first);
    polynomial_internal::trim(second);
    if(first.empty() && second.empty()) return {};
    PolynomialMatrix2x2<MOD> matrix =
        polynomial_half_gcd_internal::gcd_matrix<MOD>(first, second);
    auto [gcd, unused] = polynomial_half_gcd_internal::apply_matrix<MOD>(
        matrix, first, second
    );
    (void)unused;
    const Modint<MOD> inverse_leading = gcd.back().inv();
    for(Modint<MOD>& coefficient: gcd) coefficient *= inverse_leading;
    for(Modint<MOD>& coefficient: matrix.top_left){
        coefficient *= inverse_leading;
    }
    for(Modint<MOD>& coefficient: matrix.top_right){
        coefficient *= inverse_leading;
    }
    polynomial_internal::trim(gcd);
    polynomial_internal::trim(matrix.top_left);
    polynomial_internal::trim(matrix.top_right);
    return {
        std::move(gcd),
        std::move(matrix.top_left),
        std::move(matrix.top_right)
    };
}

template<int MOD>
Polynomial<MOD> polynomial_gcd_half(
    Polynomial<MOD> first,
    Polynomial<MOD> second
){
    return polynomial_extended_gcd_half<MOD>(
        std::move(first), std::move(second)
    ).gcd;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_HALF_GCD_HPP_INCLUDED
