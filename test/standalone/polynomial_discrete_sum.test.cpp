// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/polynomial_discrete_sum.hpp"
#include "../../src/structure/modint/fast_modint.hpp"
#include "../../src/structure/modint/modint.hpp"

namespace{

using mint = Modint998244353;

constexpr std::size_t MAX_POWER = 200000;
constexpr std::size_t MAX_POLYNOMIAL_DEGREE = 2048;

math::PolynomialDiscreteSum<mint, MAX_POWER> power_summation;
math::PolynomialDiscreteSum<mint, MAX_POLYNOMIAL_DEGREE> polynomial_summation;
math::PolynomialDiscreteSum<mint, MAX_POLYNOMIAL_DEGREE>::CoefficientArray coefficients{};

void self_check(){
    assert(power_summation.power_sum(10, 0) == mint(10));
    assert(power_summation.power_sum(10, 1) == mint(45));
    assert(power_summation.power_sum(10, 2) == mint(285));

    coefficients[0] = mint(3);
    coefficients[1] = mint(2);
    const auto antiderivative = polynomial_summation.discrete_antiderivative(
        coefficients, 2
    );
    assert(antiderivative[0] == mint(0));
    assert(antiderivative[1] == mint(2));
    assert(antiderivative[2] == mint(1));
    assert(polynomial_summation.polynomial_prefix_sum(
        coefficients, 2, 10LL
    ) == mint(120));

    math::PolynomialDiscreteSum<FastModint998244353, 5> fast_summation;
    assert(fast_summation.power_sum(8, 3) == FastModint998244353(784));

    bool thrown = false;
    try{
        (void)power_summation.power_sum(-1, 2);
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
        if(operation == 'P'){
            long long n;
            std::size_t exponent;
            std::cin >> n >> exponent;
            std::cout << power_summation.power_sum(n, exponent) << '\n';
        }else if(operation == 'F'){
            long long n;
            std::size_t exponent;
            std::cin >> n >> exponent;
            std::cout << power_summation.faulhaber(mint(n), exponent) << '\n';
        }else if(operation == 'D'){
            std::size_t coefficient_count;
            std::cin >> coefficient_count;
            for(std::size_t index = 0; index < coefficient_count; ++index){
                std::cin >> coefficients[index];
            }
            const auto result = polynomial_summation.discrete_antiderivative(
                coefficients, coefficient_count
            );
            for(std::size_t index = 0; index <= coefficient_count; ++index){
                if(index != 0) std::cout << ' ';
                std::cout << result[index];
            }
            std::cout << '\n';
        }else if(operation == 'S' || operation == 'M'){
            std::size_t coefficient_count;
            long long n;
            std::cin >> coefficient_count >> n;
            for(std::size_t index = 0; index < coefficient_count; ++index){
                std::cin >> coefficients[index];
            }
            if(operation == 'S'){
                std::cout << polynomial_summation.polynomial_prefix_sum(
                    coefficients, coefficient_count, n
                ) << '\n';
            }else{
                std::cout << polynomial_summation.polynomial_prefix_sum(
                    coefficients, coefficient_count, mint(n)
                ) << '\n';
            }
        }else{
            throw std::runtime_error("unknown polynomial discrete sum operation");
        }
    }
}
