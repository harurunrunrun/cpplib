// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/min_plus_convolution_convex_convex

#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/min_plus_convolution.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<long long> left(static_cast<std::size_t>(n));
    std::vector<long long> right(static_cast<std::size_t>(m));
    for(long long& value: left) std::cin >> value;
    for(long long& value: right) std::cin >> value;
    const auto answer = min_plus_convolution_convex_convex(left, right);
    for(std::size_t i = 0; i < answer.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << answer[i];
    }
    std::cout << '\n';
}
