// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/minimum_total_interval_cut_cost.hpp"

int main(){
    long long interval_length;
    int cut_count;
    while(std::cin >> interval_length >> cut_count){
        std::vector<long long> cut_positions(
            static_cast<std::size_t>(cut_count)
        );
        for(auto& position: cut_positions){
            std::cin >> position;
        }
        std::cout << minimum_total_interval_cut_cost(
            interval_length, cut_positions
        ) << '\n';
    }
}
