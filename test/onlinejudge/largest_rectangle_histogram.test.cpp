// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DPL_3_C

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/largest_rectangle_histogram.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<long long> heights(static_cast<std::size_t>(n));
    for(auto& height: heights) std::cin >> height;
    std::cout << largest_rectangle_histogram(heights).area << '\n';
}
