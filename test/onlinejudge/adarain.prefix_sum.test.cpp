#define PROBLEM "https://www.spoj.com/problems/ADARAIN/"

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/other/prefix_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rain_count, query_count, length;
    std::cin >> rain_count >> query_count >> length;

    std::vector<int> difference(static_cast<std::size_t>(length) + 1, 0);
    while(rain_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        difference[static_cast<std::size_t>(left)]++;
        difference[static_cast<std::size_t>(right + 1)]--;
    }

    auto coverage = std::make_unique<PrefixSum<int, 1000001>>(difference);
    while(query_count-- != 0){
        int position;
        std::cin >> position;
        std::cout << coverage->prefix_sum(position + 1) << '\n';
    }
}
