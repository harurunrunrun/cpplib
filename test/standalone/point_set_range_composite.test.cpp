// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/point_set_range_composite.hpp"

using mint = Modint998244353;

void self_test(){
    std::vector<MonoidAffine<mint>> functions{{2, 1}, {3, 4}, {1, 5}};
    PointSetRangeComposite<mint, 8> queries(functions);
    assert(queries.size() == 3);
    assert(queries.evaluate(0, 3, mint{7}) == mint{54});
    assert(queries.evaluate(1, 1, mint{9}) == mint{9});
    queries.set(1, mint{1}, mint{0});
    assert(queries.evaluate(0, 3, mint{7}) == mint{20});
}

int main(){
    self_test();

    int size, query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    std::vector<MonoidAffine<mint>> functions(
        static_cast<std::size_t>(size)
    );
    for(auto& function: functions){
        int multiplier, addend;
        std::cin >> multiplier >> addend;
        function = {multiplier, addend};
    }
    PointSetRangeComposite<mint, 256> queries(functions);
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            int index, multiplier, addend;
            std::cin >> index >> multiplier >> addend;
            queries.set(index, multiplier, addend);
        }else{
            int left, right, value;
            std::cin >> left >> right >> value;
            std::cout << queries.evaluate(left, right, mint{value}).val()
                      << '\n';
        }
    }
}
