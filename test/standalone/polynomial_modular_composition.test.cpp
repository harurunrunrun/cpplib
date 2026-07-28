// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/polynomial_modular_composition.hpp"

using mint = Modint998244353;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        std::size_t outer_size;
        std::size_t inner_size;
        std::size_t modulus_size;
        std::cin >> outer_size >> inner_size >> modulus_size;
        std::vector<mint> outer(outer_size);
        std::vector<mint> inner(inner_size);
        std::vector<mint> modulus(modulus_size);
        for(mint& value: outer) std::cin >> value;
        for(mint& value: inner) std::cin >> value;
        for(mint& value: modulus) std::cin >> value;
        const std::vector<mint> result =
            math::polynomial_compose_mod<998244353>(
                outer, inner, modulus
            );
        std::cout << result.size();
        for(const mint value: result) std::cout << ' ' << value;
        std::cout << '\n';
    }
}
