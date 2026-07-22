// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/polynomial/partial_fraction_decomposition.hpp"

using mint = Modint998244353;
using polynomial = math::Polynomial<998244353>;

polynomial add(const polynomial& left, const polynomial& right){
    polynomial result(std::max(left.size(), right.size()));
    for(std::size_t i = 0; i < left.size(); ++i) result[i] += left[i];
    for(std::size_t i = 0; i < right.size(); ++i) result[i] += right[i];
    math::polynomial_internal::trim(result);
    return result;
}

void verify_identity(
    const polynomial& numerator,
    const std::vector<polynomial>& factors
){
    const auto result = math::partial_fraction_decomposition<998244353>(
        numerator, factors
    );
    polynomial denominator{mint(1)};
    for(const polynomial& factor: factors){
        denominator = math::polynomial_multiply<998244353>(denominator, factor);
    }
    polynomial reconstructed = math::polynomial_multiply<998244353>(
        result.polynomial_part, denominator
    );
    for(std::size_t i = 0; i < factors.size(); ++i){
        const auto division = math::polynomial_divmod<998244353>(
            denominator, factors[i]
        );
        assert(division.remainder.empty());
        reconstructed = add(reconstructed,
            math::polynomial_multiply<998244353>(
                result.numerators[i], division.quotient
            )
        );
        assert(result.numerators[i].size() < factors[i].size());
    }
    polynomial expected = numerator;
    math::polynomial_internal::trim(expected);
    assert(reconstructed == expected);
}

void self_test(){
    verify_identity({mint(1), mint(2), mint(3)}, {});
    verify_identity(
        {mint(7), mint(-2), mint(4), mint(9)},
        {{mint(-1), mint(1)}, {mint(-3), mint(1)}}
    );
    verify_identity(
        {mint(5), mint(0), mint(6), mint(1), mint(8), mint(4)},
        {{mint(4), mint(-4), mint(1)}, {mint(-7), mint(1)}}
    );

    bool rejected = false;
    try{
        (void)math::partial_fraction_decomposition<998244353>(
            {mint(1)}, {{mint(-2), mint(1)}, {mint(4), mint(-4), mint(1)}}
        );
    }catch(const std::runtime_error&){
        rejected = true;
    }
    assert(rejected);

    rejected = false;
    try{
        (void)math::partial_fraction_decomposition<998244353>(
            {mint(1)}, {{mint(2)}}
        );
    }catch(const std::runtime_error&){
        rejected = true;
    }
    assert(rejected);

    std::mt19937_64 random(2026071613);
    for(int iteration = 0; iteration < 2000; ++iteration){
        const std::size_t count = 1 + random() % 7;
        std::vector<polynomial> factors;
        factors.reserve(count);
        std::vector<unsigned long long> roots;
        while(roots.size() < count){
            const unsigned long long root = 1 + random() % 100000;
            if(std::find(roots.begin(), roots.end(), root) == roots.end()){
                roots.push_back(root);
                factors.push_back({-mint(root), mint(1)});
            }
        }
        const std::size_t numerator_size = random() % (count + 9);
        polynomial numerator(numerator_size);
        for(mint& value: numerator) value = mint(random() % 998244353ULL);
        verify_identity(numerator, factors);
    }
}

void print_polynomial(const polynomial& value){
    std::cout << value.size();
    for(const mint coefficient: value) std::cout << ' ' << coefficient;
    std::cout << '\n';
}

int main(){
    std::size_t numerator_size;
    std::size_t factor_count;
    if(!(std::cin >> numerator_size >> factor_count)){
        self_test();
        return 0;
    }
    polynomial numerator(numerator_size);
    for(mint& value: numerator) std::cin >> value;
    std::vector<polynomial> factors(factor_count);
    for(polynomial& factor: factors){
        std::size_t size;
        std::cin >> size;
        factor.resize(size);
        for(mint& value: factor) std::cin >> value;
    }
    const auto result = math::partial_fraction_decomposition<998244353>(
        std::move(numerator), std::move(factors)
    );
    print_polynomial(result.polynomial_part);
    std::cout << result.numerators.size() << '\n';
    for(const polynomial& value: result.numerators) print_polynomial(value);
}
