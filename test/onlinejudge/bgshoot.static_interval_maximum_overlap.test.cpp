// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/BGSHOOT/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/structure/other/static_interval_maximum_overlap.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int animal_count;
    std::cin >> animal_count;
    std::vector<std::pair<int, int>> intervals(
        static_cast<std::size_t>(animal_count)
    );
    for(auto& [left, right]: intervals){
        std::cin >> left >> right;
    }
    StaticIntervalMaximumOverlap<int, 400000> overlap(std::move(intervals));

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        std::cout << overlap.maximum_overlap(left, right) << '\n';
    }
}
