// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_affine_range_sum.hpp"

using mint = Modint998244353;

void self_test(){
    RangeAffineRangeSum<mint, 8> queries(
        std::vector<mint>{mint{1}, mint{2}, mint{3}, mint{4}}
    );
    assert(queries.size() == 4);
    assert(queries.range_sum(0, 4) == mint{10});
    queries.apply(1, 4, mint{2}, mint{1});
    assert(queries.range_sum(0, 4) == mint{22});
    queries.apply(2, 2, mint{7}, mint{9});
    assert(queries.range_sum(2, 2) == mint{0});
}

int main(){
    self_test();

    int size, query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    std::vector<mint> values(static_cast<std::size_t>(size));
    for(auto& value: values) std::cin >> value;
    RangeAffineRangeSum<mint, 256> queries(values);
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            int left, right, multiplier, addend;
            std::cin >> left >> right >> multiplier >> addend;
            queries.apply(left, right, multiplier, addend);
        }else{
            int left, right;
            std::cin >> left >> right;
            std::cout << queries.range_sum(left, right).val() << '\n';
        }
    }
}
