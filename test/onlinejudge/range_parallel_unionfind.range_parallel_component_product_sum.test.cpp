// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_parallel_unionfind

#include <utility>
#include <iostream>
#include <vector>

#include "../../src/structure/dsu/range_parallel_component_product_sum.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    std::cin >> n >> query_count;
    std::vector<FastModint998244353> component_sum(
        static_cast<std::size_t>(n)
    );
    for(auto& value: component_sum) std::cin >> value;

    RangeParallelComponentProductSum<500000, FastModint998244353> dsu(
        std::move(component_sum)
    );
    while(query_count-- > 0){
        int length, first, second;
        std::cin >> length >> first >> second;
        std::cout << dsu.merge_ranges(
            first, first + length, second, second + length
        ) << '\n';
    }
}
