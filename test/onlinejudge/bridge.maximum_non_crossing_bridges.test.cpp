// competitive-verifier: PROBLEM https://www.spoj.com/problems/BRIDGE/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/maximum_noncrossing_bipartite_segments.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size;
        std::cin >> size;
        std::vector<int> first_coordinates(static_cast<std::size_t>(size));
        std::vector<std::pair<int, int>> segments(static_cast<std::size_t>(size));
        for(int& coordinate: first_coordinates) std::cin >> coordinate;
        for(std::size_t index = 0; index < segments.size(); ++index){
            segments[index].first = first_coordinates[index];
            std::cin >> segments[index].second;
        }
        std::cout << maximum_noncrossing_bipartite_segments(
            std::move(segments)
        ) << '\n';
    }
}
