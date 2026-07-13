// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_sum

#include <iostream>
#include <vector>

#include "../../src/structure/other/prefix_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    std::cin >> n >> query_count;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(auto& value: values) std::cin >> value;

    PrefixSum<long long, 500000> sum(values);
    while(query_count--){
        int left, right;
        std::cin >> left >> right;
        std::cout << sum.sum(left, right) << '\n';
    }
}
