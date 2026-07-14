// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/structure/wavelet_matrix/static_range_count_sum_leq.hpp"

void self_test(){
    const int int_min = std::numeric_limits<int>::min();
    const int int_max = std::numeric_limits<int>::max();
    StaticRangeCountSumLeq<int, long long, 8> matrix(
        std::vector<int>{int_min, -1, 0, int_max}
    );

    const auto all = matrix.query(0, 4, int_max);
    assert(all.count == 4);
    assert(all.sum == -2);
    const auto minimum = matrix.query(0, 4, int_min);
    assert(minimum.count == 1);
    assert(minimum.sum == static_cast<long long>(int_min));
    const auto empty = matrix.query(2, 2, int_max);
    assert(empty.count == 0);
    assert(empty.sum == 0);

    StaticRangeCountSumLeq<int, long long, 4> weighted(
        std::vector<int>{1, 2, 3},
        std::vector<long long>{10, -4, 7}
    );
    const auto weighted_result = weighted.query(0, 3, 2);
    assert(weighted_result.count == 2);
    assert(weighted_result.sum == 6);

    using UInt = unsigned int;
    const UInt uint_max = std::numeric_limits<UInt>::max();
    StaticRangeCountSumLeq<UInt, unsigned long long, 4> unsigned_matrix(
        std::vector<UInt>{0, uint_max - 1, uint_max}
    );
    const auto unsigned_all = unsigned_matrix.query(0, 3, uint_max);
    assert(unsigned_all.count == 3);
    assert(
        unsigned_all.sum ==
        static_cast<unsigned long long>(uint_max - 1) +
            static_cast<unsigned long long>(uint_max)
    );
    const auto unsigned_prefix = unsigned_matrix.query(0, 3, uint_max - 1);
    assert(unsigned_prefix.count == 2);

    bool thrown = false;
    try{
        (void)matrix.query(
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            int_max
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)matrix.query(3, 2, int_max);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int size, query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    StaticRangeCountSumLeq<int, long long, 256> matrix(values);
    for(int query = 0; query < query_count; ++query){
        int left, right, upper;
        std::cin >> left >> right >> upper;
        const auto result = matrix.query(left, right, upper);
        std::cout << result.count << ' ' << result.sum << '\n';
    }
}
