// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_parallel_unionfind

#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/structure/dsu/range_parallel_unionfind.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long mod = 998244353;
    int n, query_count;
    std::cin >> n >> query_count;
    std::vector<long long> component_sum(static_cast<std::size_t>(n));
    for(long long& value: component_sum) std::cin >> value;

    RangeParallelUnionFind<500000> dsu(n);
    long long answer = 0;
    while(query_count-- > 0){
        int length, first, second;
        std::cin >> length >> first >> second;
        dsu.merge_ranges(
            first, first + length, second, second + length,
            [&](int root, int absorbed){
                answer += component_sum[static_cast<std::size_t>(root)] *
                    component_sum[static_cast<std::size_t>(absorbed)] % mod;
                if(answer >= mod) answer -= mod;
                component_sum[static_cast<std::size_t>(root)] +=
                    component_sum[static_cast<std::size_t>(absorbed)];
                if(component_sum[static_cast<std::size_t>(root)] >= mod){
                    component_sum[static_cast<std::size_t>(root)] -= mod;
                }
            }
        );
        std::cout << answer << '\n';
    }
}
