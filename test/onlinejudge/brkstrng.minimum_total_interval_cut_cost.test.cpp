// competitive-verifier: PROBLEM https://www.spoj.com/problems/BRKSTRNG/
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/minimum_total_interval_cut_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
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
    return 0;
}
