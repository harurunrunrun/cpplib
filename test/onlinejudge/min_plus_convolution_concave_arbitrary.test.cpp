// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/min_plus_convolution_concave_arbitrary

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/min_plus_convolution_concave.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<long long> concave(static_cast<std::size_t>(n));
    std::vector<long long> arbitrary(static_cast<std::size_t>(m));
    for(long long& value: concave) std::cin >> value;
    for(long long& value: arbitrary) std::cin >> value;
    const auto answer =
        min_plus_convolution_concave_arbitrary(concave, arbitrary);
    for(std::size_t i = 0; i < answer.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << answer[i];
    }
    std::cout << '\n';
}
