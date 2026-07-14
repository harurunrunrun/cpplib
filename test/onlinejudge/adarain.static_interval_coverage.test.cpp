// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADARAIN/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/structure/other/static_interval_coverage.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rain_count, query_count, length;
    std::cin >> rain_count >> query_count >> length;

    std::vector<std::pair<int, int>> intervals;
    intervals.reserve(static_cast<std::size_t>(rain_count));
    while(rain_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        intervals.emplace_back(left, right);
    }

    StaticIntervalCoverage coverage(length, intervals);
    while(query_count-- != 0){
        int position;
        std::cin >> position;
        std::cout << coverage.coverage_at(position) << '\n';
    }
}
