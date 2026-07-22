// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/minimum_weighted_left_endpoint_partition_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    std::cin >> case_count;
    while(case_count--){
        int size, group_count;
        std::cin >> size >> group_count;
        std::vector<long long> weights(static_cast<std::size_t>(size));
        for(long long& weight: weights) std::cin >> weight;
        try{
            std::cout << minimum_weighted_left_endpoint_partition_cost(
                weights, group_count
            ) << '\n';
        }catch(const std::overflow_error&){
            std::cout << "overflow\n";
        }
    }
}
