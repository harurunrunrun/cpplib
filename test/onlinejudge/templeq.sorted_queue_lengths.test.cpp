// competitive-verifier: PROBLEM https://www.spoj.com/problems/TEMPLEQ/

#include <iostream>
#include <vector>

#include "../../src/structure/other/threshold_updated_indexed_multiset.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int queue_count, query_count;
    std::cin >> queue_count >> query_count;
    std::vector<long long> initial(static_cast<std::size_t>(queue_count));
    for(long long& length: initial) std::cin >> length;
    ThresholdUpdatedIndexedMultiset<> queues(initial);

    while(query_count--){
        int type;
        long long argument;
        std::cin >> type >> argument;
        if(type == 1){
            queues.increment(static_cast<int>(argument) - 1);
        }else if(type == 2){
            std::cout << queues.count_at_least(argument) << '\n';
        }else{
            queues.decrement_at_least(argument);
        }
    }
}
