// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/set_power_series_composition.hpp"
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
        int count;
        std::cin >> operation >> bit_count >> count;
        const std::size_t subset_count = std::size_t{1} << bit_count;
        if(operation == 'C'){
            std::vector<Mint> polynomial(static_cast<std::size_t>(count));
            std::vector<Mint> series(subset_count);
            for(Mint& value: polynomial) std::cin >> value;
            for(Mint& value: series) std::cin >> value;
            const auto answer =
                polynomial_composite_set_power_series(polynomial, series);
            for(std::size_t index = 0; index < answer.size(); ++index){
                if(index) std::cout << ' ';
                std::cout << answer[index];
            }
        }else{
            std::vector<Mint> series(subset_count), weights(subset_count);
            for(Mint& value: series) std::cin >> value;
            for(Mint& value: weights) std::cin >> value;
            const auto answer = set_power_series_power_projection(
                series, weights, static_cast<std::size_t>(count)
            );
            for(std::size_t index = 0; index < answer.size(); ++index){
                if(index) std::cout << ' ';
                std::cout << answer[index];
            }
        }
        std::cout << '\n';
    }
}
