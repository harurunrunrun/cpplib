// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/subset_convolution

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/subset_convolution.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int bit_count;
    std::cin >> bit_count;
    const std::size_t size = std::size_t{1} << bit_count;
    std::vector<FastModint998244353> left(size), right(size);
    for(auto& value: left) std::cin >> value;
    for(auto& value: right) std::cin >> value;
    const auto answer = subset_convolution(left, right);
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
