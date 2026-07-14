// competitive-verifier: PROBLEM https://www.spoj.com/problems/POSTERS/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/count_visible_overlaid_intervals.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count--){
        int n;
        std::cin >> n;
        std::vector<std::pair<int, int>> intervals(static_cast<std::size_t>(n));
        for(auto& [left, right]: intervals){
            std::cin >> left >> right;
        }
        std::cout << count_visible_overlaid_intervals<int, 160000>(intervals)
                  << '\n';
    }
}
