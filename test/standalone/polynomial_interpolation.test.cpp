// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/polynomial_interpolation.hpp"
#include "../../src/structure/modint/fast_modint.hpp"
#include "../../src/structure/modint/modint.hpp"

namespace{

using mint = Modint998244353;

constexpr std::size_t LARGE_MAX_DEGREE = 200000;
constexpr std::size_t SMALL_MAX_DEGREE = 64;

math::PolynomialInterpolation<mint, LARGE_MAX_DEGREE> large_interpolation;
math::PolynomialInterpolation<mint, SMALL_MAX_DEGREE> small_interpolation;
math::PolynomialInterpolation<mint, LARGE_MAX_DEGREE>::ValueArray large_values{};
math::PolynomialInterpolation<mint, SMALL_MAX_DEGREE>::ValueArray small_xs{};
math::PolynomialInterpolation<mint, SMALL_MAX_DEGREE>::ValueArray small_ys{};

void self_check(){
    std::array<mint, 3> coefficients{mint(2), mint(3), mint(4)};
    assert(math::polynomial_evaluate(coefficients, 3, mint(5)) == mint(117));
    assert(math::polynomial_evaluate(coefficients, 0, mint(5)) == mint(0));

    math::PolynomialInterpolation<FastModint998244353, 3> fast_interpolation;
    math::PolynomialInterpolation<FastModint998244353, 3>::ValueArray values{
        FastModint998244353(1), FastModint998244353(4),
        FastModint998244353(9), FastModint998244353(16)
    };
    assert(fast_interpolation.evaluate_consecutive(
        values, 4, FastModint998244353(9)
    ) == FastModint998244353(100));

    bool thrown = false;
    try{
        (void)small_interpolation.evaluate_consecutive(small_ys, 0, mint(0));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)small_interpolation.evaluate_equidistant(
            small_ys, 1, mint(0), mint(0), mint(0)
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    small_xs[0] = mint(7);
    small_xs[1] = mint(7);
    try{
        (void)small_interpolation.interpolate_coefficients(small_xs, small_ys, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_check();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    for(int query = 0; query < query_count; ++query){
        char operation;
        std::cin >> operation;
        if(operation == 'E'){
            std::size_t sample_count;
            long long first_value, step_value, x_value;
            std::cin >> sample_count >> first_value >> step_value >> x_value;
            for(std::size_t index = 0; index < sample_count; ++index){
                std::cin >> large_values[index];
            }
            std::cout << large_interpolation.evaluate_equidistant(
                large_values,
                sample_count,
                mint(first_value),
                mint(step_value),
                mint(x_value)
            ) << '\n';
        }else if(operation == 'C'){
            std::size_t sample_count;
            std::cin >> sample_count;
            for(std::size_t index = 0; index < sample_count; ++index){
                std::cin >> small_xs[index];
            }
            for(std::size_t index = 0; index < sample_count; ++index){
                std::cin >> small_ys[index];
            }
            const auto coefficients = small_interpolation.interpolate_coefficients(
                small_xs, small_ys, sample_count
            );
            for(std::size_t index = 0; index < sample_count; ++index){
                if(index != 0) std::cout << ' ';
                std::cout << coefficients[index];
            }
            std::cout << '\n';
        }else if(operation == 'A'){
            std::size_t sample_count;
            long long x_value;
            std::cin >> sample_count >> x_value;
            for(std::size_t index = 0; index < sample_count; ++index){
                std::cin >> small_xs[index];
            }
            for(std::size_t index = 0; index < sample_count; ++index){
                std::cin >> small_ys[index];
            }
            std::cout << small_interpolation.evaluate(
                small_xs, small_ys, sample_count, mint(x_value)
            ) << '\n';
        }else{
            throw std::runtime_error("unknown polynomial interpolation operation");
        }
    }
}
