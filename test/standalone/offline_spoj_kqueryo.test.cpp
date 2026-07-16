// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/kqueryo.static_range_greater_count.test.cpp"
#undef main

#include <cassert>
#include <limits>
#include <vector>

void generic_boundary_test(){
    const std::vector<long long> values = {
        std::numeric_limits<long long>::lowest(),
        -4,
        -4,
        0,
        std::numeric_limits<long long>::max()
    };
    StaticRangeGreaterCount<long long, 8, 3> range_count(values);
    assert(range_count.size() == 5);
    assert(range_count.count_greater(0, 5, -5) == 4);
    assert(range_count.count_greater(1, 4, -4) == 1);
    assert(range_count.count_greater(2, 2, 0) == 0);
    assert(
        range_count.count_greater(
            0,
            5,
            std::numeric_limits<long long>::max()
        ) == 0
    );
}

int main(){
    generic_boundary_test();
    return onlinejudge_adapter_main();
}
