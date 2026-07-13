// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sqrt_of_formal_power_series_sparse

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/sparse_formal_power_series.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, k;
    std::cin >> n >> k;
    std::vector<SparseFormalPowerSeriesTerm<998244353>> terms(static_cast<std::size_t>(k));
    for(auto& [degree, coefficient]: terms) std::cin >> degree >> coefficient;
    const auto answer = sparse_fps_sqrt(terms, static_cast<std::size_t>(n));
    if(!answer){
        std::cout << "-1\n";
        return 0;
    }
    for(int index = 0; index < n; ++index){
        if(index != 0) std::cout << ' ';
        std::cout << (*answer)[static_cast<std::size_t>(index)];
    }
    std::cout << '\n';
}
