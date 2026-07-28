// competitive-verifier: STANDALONE

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
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        char operation;
        std::uint64_t maximum;
        std::cin >> operation >> maximum;
        const std::size_t count =
            math::enumerate_quotient_values(
                static_cast<long long>(maximum)
            ).size();
        std::vector<Mint> first(count);
        for(Mint& value: first) std::cin >> value;
        std::vector<Mint> answer;
        if(operation == 'C'){
            std::vector<Mint> second(count);
            for(Mint& value: second) std::cin >> value;
            answer = math::dirichlet_prefix_convolution(
                maximum, first, second
            );
        }else{
            answer = math::dirichlet_prefix_inverse(maximum, first);
        }
        for(std::size_t index = 0; index < count; ++index){
            if(index) std::cout << ' ';
            std::cout << answer[index];
        }
        std::cout << '\n';
    }
}
