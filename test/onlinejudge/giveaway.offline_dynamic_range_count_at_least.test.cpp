// competitive-verifier: PROBLEM https://www.spoj.com/problems/GIVEAWAY/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/offline_query/offline_dynamic_range_count_at_least.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    OfflineDynamicRangeCountAtLeast queries(std::move(values));

    int query_count;
    std::cin >> query_count;
    for(int query_index = 0; query_index < query_count; ++query_index){
        int type, first, second;
        std::cin >> type >> first >> second;
        --first;
        if(type == 0){
            int threshold;
            std::cin >> threshold;
            queries.add_count_at_least(first, second, threshold);
        }else{
            queries.add_set(first, second);
        }
    }
    for(int answer: queries.solve()) std::cout << answer << '\n';
}
