// competitive-verifier: PROBLEM https://www.spoj.com/problems/ABA12C/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/dynamic_programming/minimum_exact_weight_cost_with_item_limit.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){
        int maximum_item_count,target_weight;
        std::cin>>maximum_item_count>>target_weight;
        std::vector<long long> cost_by_weight(target_weight);
        for(long long& cost:cost_by_weight) std::cin>>cost;
        std::cout<<minimum_exact_weight_cost_with_item_limit(
            target_weight,maximum_item_count,cost_by_weight
        )<<'\n'; }
}
