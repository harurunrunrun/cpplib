// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc231/tasks/abc231_f

#include <iostream>
#include <vector>

#include "../../src/algorithm/range/count_dominance_pairs.hpp"

int main(){
    int n;
    std::cin >> n;
    std::vector<int> a(static_cast<std::size_t>(n));
    std::vector<int> b(static_cast<std::size_t>(n));
    for(int& value: a) std::cin >> value;
    for(int& value: b) std::cin >> value;
    std::cout << count_dominance_pairs(a, b) << '\n';
}
