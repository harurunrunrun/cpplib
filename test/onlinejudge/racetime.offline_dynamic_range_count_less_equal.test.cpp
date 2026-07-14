// competitive-verifier: PROBLEM https://www.spoj.com/problems/RACETIME/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/offline_dynamic_range_count_less_equal.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, query_count;
    std::cin >> n >> query_count;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    OfflineDynamicRangeCountLessEqual<long long> queries(std::move(values));
    for(int query = 0; query < query_count; query++){
        char type;
        std::cin >> type;
        if(type == 'M'){
            int index;
            long long value;
            std::cin >> index >> value;
            queries.add_set(index - 1, value);
        }else{
            int left, right;
            long long threshold;
            std::cin >> left >> right >> threshold;
            queries.add_count_less_equal(left - 1, right, threshold);
        }
    }
    for(int answer: queries.solve()) std::cout << answer << '\n';
}
