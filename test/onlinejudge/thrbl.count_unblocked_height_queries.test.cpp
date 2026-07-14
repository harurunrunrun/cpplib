// competitive-verifier: PROBLEM https://www.spoj.com/problems/THRBL/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/count_unblocked_height_queries.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int city_count, ball_count;
    std::cin >> city_count >> ball_count;

    std::vector<int> height(static_cast<std::size_t>(city_count));
    for(int& value : height){
        std::cin >> value;
    }
    std::vector<std::pair<int, int>> queries;
    queries.reserve(static_cast<std::size_t>(ball_count));
    while(ball_count--){
        int from, to;
        std::cin >> from >> to;
        queries.emplace_back(from - 1, to - 1);
    }
    std::cout << count_unblocked_height_queries(height, queries) << '\n';
}
