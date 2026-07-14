// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_affine_point_get.hpp"

using mint = Modint998244353;

void self_test(){
    RangeAffinePointGet<mint, 8> queries(
        std::vector<mint>{mint{1}, mint{2}, mint{3}}
    );
    assert(queries.size() == 3);
    queries.apply(0, 2, mint{2}, mint{1});
    queries.apply(1, 3, mint{3}, mint{4});
    assert(queries.get(0) == mint{3});
    assert(queries.get(1) == mint{19});
    assert(queries.get(2) == mint{13});
}

int main(){
    self_test();

    int size, query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    std::vector<mint> values(static_cast<std::size_t>(size));
    for(auto& value: values) std::cin >> value;
    RangeAffinePointGet<mint, 256> queries(values);
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            int left, right, multiplier, addend;
            std::cin >> left >> right >> multiplier >> addend;
            queries.apply(left, right, multiplier, addend);
        }else{
            int index;
            std::cin >> index;
            std::cout << queries.get(index).val() << '\n';
        }
    }
}
