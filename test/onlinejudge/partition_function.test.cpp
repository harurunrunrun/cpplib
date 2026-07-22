// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/partition_function

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/partition_function.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t maximum_sum;
    std::cin >> maximum_sum;
    const auto answer = partition_numbers<998244353>(maximum_sum);
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
