// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/structure/dsu/range_parallel_component_product_sum.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

void self_test(){
    RangeParallelComponentProductSum<8, long long> dsu({1, 2, 3, 4});
    assert(dsu.size() == 4);
    assert(dsu.groups() == 4);
    assert(dsu.product_sum() == 0);
    assert(dsu.merge_ranges(0, 2, 2, 4) == 11);
    assert(dsu.groups() == 2);
    assert(dsu.component_sum(0) == 4);
    assert(dsu.component_sum(1) == 6);
    assert(dsu.same(0, 2));
    assert(dsu.merge_ranges(0, 1, 1, 2) == 35);
    assert(dsu.groups() == 1);

    bool thrown = false;
    try{ RangeParallelComponentProductSum<2, int> invalid({1, 2, 3}); }
    catch(const std::length_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    self_test();
    int size, query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    std::vector<FastModint998244353> weights(static_cast<std::size_t>(size));
    for(auto& value: weights) std::cin >> value;
    RangeParallelComponentProductSum<256, FastModint998244353> dsu(
        std::move(weights)
    );
    while(query_count-- > 0){
        int length, first, second;
        std::cin >> length >> first >> second;
        std::cout << dsu.merge_ranges(
            first, first + length, second, second + length
        ) << '\n';
    }
}
