// competitive-verifier: PROBLEM https://www.spoj.com/problems/THRBL/

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/other/sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr MaxMonoid<int> maximum_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int city_count, ball_count;
    std::cin >> city_count >> ball_count;

    std::vector<int> height(static_cast<std::size_t>(city_count));
    for(int& value : height){
        std::cin >> value;
    }
    auto table = std::make_unique<SparseTable<maximum_monoid, 50000>>(height);

    int successful = 0;
    while(ball_count--){
        int from, to;
        std::cin >> from >> to;
        from--;
        to--;
        const int left = std::min(from, to);
        const int right = std::max(from, to);
        if(right - left <= 1
            || table->prod(left + 1, right) <= height[from]){
            successful++;
        }
    }
    std::cout << successful << '\n';
}
