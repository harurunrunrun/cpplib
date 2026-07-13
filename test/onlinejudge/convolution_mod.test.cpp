// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/convolution_mod

#include <iostream>
#include <vector>
#include "../../src/algorithm/fft/convolution.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<Modint998244353> a(static_cast<std::size_t>(n));
    std::vector<Modint998244353> b(static_cast<std::size_t>(m));
    for(auto& value: a) std::cin >> value;
    for(auto& value: b) std::cin >> value;
    const auto answer = convolution_ntt(std::move(a), std::move(b));
    for(std::size_t i = 0; i < answer.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << answer[i];
    }
    std::cout << '\n';
}
