// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_affine_range_sum_large_array.hpp"

using mint = Modint998244353;

void self_test(){
    RangeAffineRangeSumLargeArray<mint, 1'000'000'000> queries(
        1'000'000'000
    );
    assert(queries.size() == 1'000'000'000);
    queries.apply(0, 1'000'000'000, mint{2}, mint{3});
    assert(
        queries.range_sum(0, 1'000'000'000)
        == mint{3} * mint{1'000'000'000}
    );
    queries.apply(4, 7, mint{5}, mint{1});
    assert(queries.range_sum(3, 8) == mint{54});
}

int main(){
    self_test();

    long long size;
    int query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    RangeAffineRangeSumLargeArray<mint, 1'000'000'000> queries(size);
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            long long left, right;
            int multiplier, addend;
            std::cin >> left >> right >> multiplier >> addend;
            queries.apply(left, right, multiplier, addend);
        }else{
            long long left, right;
            std::cin >> left >> right;
            std::cout << queries.range_sum(left, right).val() << '\n';
        }
    }
}
