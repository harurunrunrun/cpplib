// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/deque_operate_all_composite

#include <iostream>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/range_query/aggregation/aggregate_deque.hpp"
#include "../../src/structure/types/monoid/common_monoids.hpp"

using Mint = Modint998244353;
constexpr AffineCompositionMonoid<Mint> affine_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    std::cin >> query_count;
    AggregateDeque<affine_monoid> deque(
        static_cast<std::size_t>(query_count)
    );
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0 || type == 1){
            int multiplier;
            int addend;
            std::cin >> multiplier >> addend;
            const MonoidAffine<Mint> affine{
                Mint(multiplier), Mint(addend)
            };
            if(type == 0) deque.push_front(affine);
            else deque.push_back(affine);
        }else if(type == 2){
            deque.pop_front();
        }else if(type == 3){
            deque.pop_back();
        }else{
            int argument;
            std::cin >> argument;
            const auto affine = deque.fold();
            std::cout << affine.multiplier * Mint(argument) + affine.addend
                      << '\n';
        }
    }
}
