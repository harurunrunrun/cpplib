// competitive-verifier: PROBLEM https://www.spoj.com/problems/NKLEAVES/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/minimum_weighted_left_endpoint_partition_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int element_count, group_count;
    if(!(std::cin >> element_count >> group_count)) return 0;
    std::vector<long long> weights(
        static_cast<std::size_t>(element_count)
    );
    for(long long& weight: weights) std::cin >> weight;
    std::cout << minimum_weighted_left_endpoint_partition_cost(
        weights, group_count
    ) << '\n';
}
