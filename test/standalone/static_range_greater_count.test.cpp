// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#include "../../src/structure/wavelet_matrix/static_range_greater_count.hpp"

void self_test(){
    const std::vector<int> values = {
        std::numeric_limits<int>::lowest(),
        -3,
        -3,
        0,
        8,
        std::numeric_limits<int>::max()
    };
    StaticRangeGreaterCount<int, 8, 3> range_count(values);
    assert(range_count.size() == 6);
    assert(range_count.count_greater(0, 6, -4) == 5);
    assert(range_count.count_greater(1, 5, -3) == 2);
    assert(range_count.count_greater(2, 2, 100) == 0);
    assert(
        range_count.count_greater(
            0,
            6,
            std::numeric_limits<int>::max()
        ) == 0
    );
}

int main(){
    self_test();

    int size;
    if(!(std::cin >> size)) return 0;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    StaticRangeGreaterCount<int, 30000, 15> range_count(values);

    int query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        int left, right, threshold;
        std::cin >> left >> right >> threshold;
        std::cout
            << range_count.count_greater(left, right, threshold)
            << '\n';
    }
}
