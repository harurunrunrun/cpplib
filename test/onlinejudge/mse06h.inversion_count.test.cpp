// competitive-verifier: PROBLEM https://www.spoj.com/problems/MSE06H/

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/inversion_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int case_number = 1; case_number <= test_count; ++case_number){
        int east_city_count;
        int west_city_count;
        int road_count;
        std::cin >> east_city_count >> west_city_count >> road_count;
        (void)east_city_count;
        (void)west_city_count;

        std::vector<std::pair<int, int>> roads(
            static_cast<std::size_t>(road_count)
        );
        for(auto& [east, west]: roads) std::cin >> east >> west;
        std::sort(roads.begin(), roads.end());

        std::vector<int> west_cities;
        west_cities.reserve(roads.size());
        for(const auto [east, west]: roads){
            (void)east;
            west_cities.push_back(west);
        }
        std::cout << "Test case " << case_number << ": "
                  << inversion_count(west_cities) << '\n';
    }
}
