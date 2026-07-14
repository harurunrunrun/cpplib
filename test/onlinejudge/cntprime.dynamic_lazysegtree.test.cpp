// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/CNTPRIME/

#include <iostream>
#include <memory>

#include "../../src/algorithm/math/prime_sieve.hpp"
#include "../../src/structure/segtree/dynamic_lazysegtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

namespace{

constexpr AssignSumMonoidAct<int> assign_sum{};

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto sieve = std::make_unique<math::PrimeSieve<1000000>>();
    int test_count;
    std::cin >> test_count;
    for(int test_case = 1; test_case <= test_count; ++test_case){
        int size, query_count;
        std::cin >> size >> query_count;
        auto tree = std::make_unique<
            DynamicLazySegtree<assign_sum, 10000>
        >(size);
        for(int index = 0; index < size; ++index){
            int value;
            std::cin >> value;
            tree->set(index, static_cast<int>(sieve->is_prime(value)));
        }

        std::cout << "Case " << test_case << ":\n";
        while(query_count-- != 0){
            int type, left, right;
            std::cin >> type >> left >> right;
            --left;
            if(type == 0){
                int value;
                std::cin >> value;
                tree->apply(
                    left,
                    right,
                    MonoidAssignment<int>{true, static_cast<int>(sieve->is_prime(value))}
                );
            }else{
                std::cout << tree->prod(left, right) << '\n';
            }
        }
    }
}
