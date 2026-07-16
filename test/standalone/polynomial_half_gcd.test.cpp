// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/polynomial_half_gcd.hpp"

namespace{

constexpr int modulus = 998244353;
using mint = Modint<modulus>;
using Polynomial = math::Polynomial<modulus>;

void self_test(){
    const Polynomial first{2, 3, 1};
    const Polynomial second{1, 1};
    const auto result = math::polynomial_extended_gcd_half<modulus>(first, second);
    assert(result.gcd == Polynomial({mint(1), mint(1)}));
    assert(math::polynomial_add<modulus>(
        math::polynomial_multiply<modulus>(first, result.left_coefficient),
        math::polynomial_multiply<modulus>(second, result.right_coefficient)
    ) == result.gcd);
    assert(math::polynomial_gcd_half<modulus>({}, {}) == Polynomial{});
    assert(math::polynomial_gcd_half<modulus>({}, {0, 6}) == Polynomial({0, 1}));

    bool threw = false;
    try{
        (void)math::polynomial_half_gcd<modulus>({1}, {1, 1});
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

void print(const Polynomial& polynomial){
    std::cout << polynomial.size();
    for(const mint coefficient: polynomial) std::cout << ' ' << coefficient;
    std::cout << '\n';
}

} // namespace

int main(){
    self_test();
    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::size_t first_size, second_size;
        std::cin >> first_size >> second_size;
        Polynomial first(first_size), second(second_size);
        for(mint& coefficient: first) std::cin >> coefficient;
        for(mint& coefficient: second) std::cin >> coefficient;
        const auto result =
            math::polynomial_extended_gcd_half<modulus>(first, second);
        assert(math::polynomial_add<modulus>(
            math::polynomial_multiply<modulus>(first, result.left_coefficient),
            math::polynomial_multiply<modulus>(second, result.right_coefficient)
        ) == result.gcd);
        if(first.size() > second.size() && !first.empty()){
            const auto matrix = math::polynomial_half_gcd<modulus>(first, second);
            const auto reduced = math::apply_polynomial_matrix<modulus>(
                matrix, first, second
            );
            const std::size_t target_degree = (first.size() - 1 + 1) / 2;
            assert(reduced.second.empty() ||
                   reduced.second.size() - 1 < target_degree);
        }
        print(result.gcd);
    }
}
