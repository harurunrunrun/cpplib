// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/maximum_interval_overlap.hpp"

void self_test(){
    using Interval = std::pair<int, int>;
    assert(maximum_interval_overlap<int>({}) == 0);
    assert(maximum_interval_overlap<int>({{1, 1}}) == 0);
    assert(maximum_interval_overlap<int>(
        {{1, 2}, {2, 3}}, IntervalEndpointPolicy::half_open
    ) == 1);
    assert(maximum_interval_overlap<int>(
        {{1, 2}, {2, 3}}, IntervalEndpointPolicy::closed
    ) == 2);
    assert(maximum_interval_overlap<int>(
        {{1, 7}, {2, 4}, {6, 9}, {3, 8}, {5, 10}}
    ) == 4);

    bool thrown = false;
    try{
        (void)maximum_interval_overlap<int>({Interval{2, 1}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)maximum_interval_overlap<int>(
            {}, static_cast<IntervalEndpointPolicy>(-1)
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int policy, interval_count;
    if(!(std::cin >> policy >> interval_count)) return 0;
    std::vector<std::pair<long long, long long>> intervals(
        static_cast<std::size_t>(interval_count)
    );
    for(auto& [left, right]: intervals) std::cin >> left >> right;
    const auto endpoint_policy = policy == 0
        ? IntervalEndpointPolicy::half_open
        : IntervalEndpointPolicy::closed;
    std::cout << maximum_interval_overlap(intervals, endpoint_policy) << '\n';
}
