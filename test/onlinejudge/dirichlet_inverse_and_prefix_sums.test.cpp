// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dirichlet_inverse_and_prefix_sums

#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/number_theory/arithmetic/enumerate_quotient_values.hpp"
#include "../../src/algorithm/math/number_theory/multiplicative/dirichlet_prefix_convolution.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Mint = Modint<998244353>;
    int case_count;
    std::cin >> case_count;
    while(case_count--){
        std::uint64_t maximum;
        std::cin >> maximum;
        const std::size_t count =
            math::enumerate_quotient_values(
                static_cast<long long>(maximum)
            ).size();
        std::vector<Mint> prefix(count);
        for(Mint& value: prefix) std::cin >> value;
        const auto answer = math::dirichlet_prefix_inverse(maximum, prefix);
        for(std::size_t index = 0; index < count; ++index){
            if(index) std::cout << ' ';
            std::cout << answer[index];
        }
        std::cout << '\n';
    }
}
