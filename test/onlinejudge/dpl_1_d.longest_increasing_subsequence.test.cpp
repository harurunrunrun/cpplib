// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DPL_1_D

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/sequence/longest_increasing_subsequence.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(auto& value: values) std::cin >> value;
    std::cout << longest_increasing_subsequence_length(values) << '\n';
}
