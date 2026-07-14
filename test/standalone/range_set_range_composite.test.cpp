// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_set_range_composite.hpp"

using mint = Modint998244353;

void self_test(){
    std::vector<MonoidAffine<mint>> functions{{2, 1}, {3, 4}, {1, 5}};
    RangeSetRangeComposite<mint, 8> queries(functions);
    assert(queries.size() == 3);
    assert(queries.evaluate(0, 3, mint{7}) == mint{54});
    queries.assign(0, 2, mint{2}, mint{3});
    assert(queries.evaluate(0, 3, mint{1}) == mint{18});
    assert(queries.evaluate(2, 2, mint{9}) == mint{9});
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
    RangeSetRangeComposite<mint, 256> queries(functions);
    while(query_count-- > 0){
        int type;
        std::cin >> type;
        if(type == 0){
            int left, right, multiplier, addend;
            std::cin >> left >> right >> multiplier >> addend;
            queries.assign(left, right, multiplier, addend);
        }else{
            int left, right, value;
            std::cin >> left >> right >> value;
            std::cout << queries.evaluate(left, right, mint{value}).val()
                      << '\n';
        }
    }
}
