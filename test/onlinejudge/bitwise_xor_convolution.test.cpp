// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/bitwise_xor_convolution

#include <iostream>
#include <vector>
#include "../../src/algorithm/fft/bitwise_convolution.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int log;
    std::cin >> log;
    const int n = 1 << log;
    std::vector<Modint998244353> a(static_cast<std::size_t>(n));
    std::vector<Modint998244353> b(static_cast<std::size_t>(n));
    for(auto& value: a) std::cin >> value;
    for(auto& value: b) std::cin >> value;
    const auto answer = xor_convolution(std::move(a), std::move(b));
    for(int i = 0; i < n; ++i){
        if(i != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
}
