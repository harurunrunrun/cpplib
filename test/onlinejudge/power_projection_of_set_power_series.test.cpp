// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/power_projection_of_set_power_series

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/set_power_series_composition.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Mint = Modint<998244353>;
    int bit_count;
    int output_count;
    std::cin >> bit_count >> output_count;
    const std::size_t subset_count = std::size_t{1} << bit_count;
    std::vector<Mint> series(subset_count), weights(subset_count);
    for(Mint& value: series) std::cin >> value;
    for(Mint& value: weights) std::cin >> value;
    const auto answer = set_power_series_power_projection(
        series,
        weights,
        static_cast<std::size_t>(output_count)
    );
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
