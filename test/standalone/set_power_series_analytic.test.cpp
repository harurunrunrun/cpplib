// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/set_power_series_analytic.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Mint = Modint<998244353>;
    int case_count;
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        char operation;
        int bit_count;
        std::cin >> operation >> bit_count;
        const std::size_t subset_count = std::size_t{1} << bit_count;
        std::vector<Mint> series(subset_count);
        for(Mint& coefficient: series){
            long long value;
            std::cin >> value;
            coefficient = Mint{value};
        }
        const auto result = operation == 'E'
            ? set_power_series_exponential(series)
            : set_power_series_logarithm(series);
        for(std::size_t subset = 0; subset < subset_count; ++subset){
            if(subset) std::cout << ' ';
            std::cout << result[subset].val();
        }
        std::cout << '\n';
    }
}
