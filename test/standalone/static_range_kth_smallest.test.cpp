// competitive-verifier: STANDALONE

#include <cassert>
#include <climits>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/static_range_kth_smallest.hpp"

void self_test(){
    {
        StaticRangeKthSmallest<int, 8> statistics(
            std::vector<int>{INT_MAX, 0, INT_MIN, 0}
        );
        assert(statistics.size() == 4);
        assert(!statistics.empty());
        assert(statistics.kth_smallest(0, 4, 0) == INT_MIN);
        assert(statistics.kth_smallest(0, 4, 3) == INT_MAX);
        assert(statistics.kth_smallest(1, 3, 0) == INT_MIN);
        assert(statistics.kth_smallest(1, 4, 1) == 0);

        for(const auto& query: std::vector<std::vector<int>>{
                {1, 1, 0}, {2, 1, 0}, {-1, 1, 0},
                {0, 5, 0}, {0, 4, -1}, {0, 4, 4}}){
            bool thrown = false;
            try{
                (void)statistics.kth_smallest(query[0], query[1], query[2]);
            }catch(const std::out_of_range&){
                thrown = true;
            }
            assert(thrown);
        }
    }
    {
        StaticRangeKthSmallest<int, 8> empty(std::vector<int>{});
        assert(empty.empty());
        bool thrown = false;
        try{
            (void)empty.kth_smallest(0, 0, 0);
        }catch(const std::out_of_range&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        bool thrown = false;
        try{
            StaticRangeKthSmallest<int, 2> too_large(
                std::vector<int>{1, 2, 3}
            );
            (void)too_large;
        }catch(const std::length_error&){
            thrown = true;
        }
        assert(thrown);
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    if(!(std::cin >> size >> query_count)){
        self_test();
        return 0;
    }
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values){
        std::cin >> value;
    }
    StaticRangeKthSmallest<long long, 512> statistics(values);
    while(query_count-- != 0){
        int left, right, order;
        std::cin >> left >> right >> order;
        std::cout << statistics.kth_smallest(left, right, order) << '\n';
    }
}
