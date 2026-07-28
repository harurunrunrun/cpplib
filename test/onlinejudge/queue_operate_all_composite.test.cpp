// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/queue_operate_all_composite

#include <iostream>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/range_query/aggregation/aggregate_queue.hpp"
#include "../../src/structure/types/monoid/common_monoids.hpp"

using Mint = Modint998244353;
constexpr AffineCompositionMonoid<Mint> affine_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    std::cin >> query_count;
    AggregateQueue<affine_monoid> queue(
        static_cast<std::size_t>(query_count)
    );
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            int multiplier;
            int addend;
            std::cin >> multiplier >> addend;
            queue.push({Mint(multiplier), Mint(addend)});
        }else if(type == 1){
            queue.pop();
        }else{
            int argument;
            std::cin >> argument;
            const auto affine = queue.fold();
            std::cout << affine.multiplier * Mint(argument) + affine.addend
                      << '\n';
        }
    }
}
