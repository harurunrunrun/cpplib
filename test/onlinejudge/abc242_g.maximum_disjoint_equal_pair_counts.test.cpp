// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc242/tasks/abc242_g

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/maximum_disjoint_equal_pair_counts.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;

    int query_count;
    std::cin >> query_count;
    std::vector<std::pair<int, int>> ranges(static_cast<std::size_t>(query_count));
    for(auto& [left, right]: ranges){
        std::cin >> left >> right;
        --left;
    }

    const auto answers = maximum_disjoint_equal_pair_counts(values, ranges);
    for(int answer: answers) std::cout << answer << '\n';
}
