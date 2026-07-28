// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/polynomial_factorization_finite_field.hpp"

using mint = Modint998244353;

void check_small_characteristics(){
    {
        using mint2 = Modint<2>;
        const auto factorization =
            math::polynomial_factorization_finite_field<2>(
                std::vector<mint2>{1, 1, 0, 0, 0, 0, 1, 1}
            );
        assert(factorization.leading_coefficient == mint2(1));
        assert(factorization.factors.size() == 2);
        assert(factorization.factors[0].polynomial ==
            std::vector<mint2>({1, 1}));
        assert(factorization.factors[0].multiplicity == 3);
        assert(factorization.factors[1].polynomial ==
            std::vector<mint2>({1, 1, 1}));
        assert(factorization.factors[1].multiplicity == 2);
    }
    {
        using mint3 = Modint<3>;
        const auto factorization =
            math::polynomial_factorization_finite_field<3>(
                std::vector<mint3>{1, 0, 0, 1}
            );
        assert(factorization.factors.size() == 1);
        assert(factorization.factors[0].polynomial ==
            std::vector<mint3>({1, 1}));
        assert(factorization.factors[0].multiplicity == 3);
    }
}

int main(){
    check_small_characteristics();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<mint> polynomial(size);
        for(mint& coefficient: polynomial) std::cin >> coefficient;
        const auto factorization =
            math::polynomial_factorization_finite_field<998244353>(
                std::move(polynomial)
            );
        std::cout << factorization.leading_coefficient << ' '
            << factorization.factors.size() << '\n';
        for(const auto& factor: factorization.factors){
            std::cout << factor.multiplicity << ' '
                << factor.polynomial.size();
            for(const mint coefficient: factor.polynomial){
                std::cout << ' ' << coefficient;
            }
            std::cout << '\n';
        }
    }
}
