// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/longest_increasing_subsequence

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/longest_increasing_subsequence.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(auto& value: values) std::cin >> value;
    const auto answer = longest_increasing_subsequence(values);
    std::cout << answer.size() << '\n';
    for(std::size_t i = 0; i < answer.size(); i++){
        if(i != 0) std::cout << ' ';
        std::cout << answer[i];
    }
    std::cout << '\n';
}
