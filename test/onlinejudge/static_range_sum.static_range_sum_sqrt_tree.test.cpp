// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_sum

#include <iostream>
#include <vector>

#include "../../src/structure/other/static_range_sum_sqrt_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    StaticRangeSumSqrtTree<long long, 500000> tree(values);
    while(q--){
        int left, right;
        std::cin >> left >> right;
        std::cout << tree.sum(left, right) << '\n';
    }
}
