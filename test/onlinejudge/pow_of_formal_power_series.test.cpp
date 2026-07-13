// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/pow_of_formal_power_series

#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/formal_power_series.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    long long exponent;
    std::cin >> n >> exponent;
    std::vector<Modint998244353> values(static_cast<std::size_t>(n));
    for(auto& value: values) std::cin >> value;
    const auto answer = fps_pow(values, exponent, values.size());
    for(int i = 0; i < n; ++i){
        if(i != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
}
