// competitive-verifier: PROBLEM https://www.spoj.com/problems/BYTESE2/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/maximum_interval_overlap.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int interval_count;
        std::cin >> interval_count;
        std::vector<std::pair<int, int>> intervals(
            static_cast<std::size_t>(interval_count)
        );
        for(auto& [entry, exit]: intervals) std::cin >> entry >> exit;
        std::cout << maximum_interval_overlap(intervals) << '\n';
    }
}
