// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/point_set_range_composite_large_array.hpp"

using mint = Modint998244353;

void self_test(){
    PointSetRangeCompositeLargeArray<mint, 1'000'000'000> queries(
        1'000'000'000
    );
    assert(queries.size() == 1'000'000'000);
    assert(queries.evaluate(2, 2, mint{11}) == mint{11});
    queries.set(5, mint{2}, mint{3});
    queries.set(999'999'999, mint{4}, mint{1});
    assert(queries.evaluate(0, 6, mint{7}) == mint{17});
    assert(queries.evaluate(5, 1'000'000'000, mint{7}) == mint{69});
}

int main(){
    self_test();

    long long size;
    int query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    PointSetRangeCompositeLargeArray<mint, 1'000'000'000> queries(size);
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            long long index;
            int multiplier, addend;
            std::cin >> index >> multiplier >> addend;
            queries.set(index, multiplier, addend);
        }else{
            long long left, right;
            int value;
            std::cin >> left >> right >> value;
            std::cout << queries.evaluate(left, right, mint{value}).val()
                      << '\n';
        }
    }
}
