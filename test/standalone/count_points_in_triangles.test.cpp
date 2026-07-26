// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/count_points_in_triangles.hpp"

void test_invalid_indices(){
    const std::vector<std::pair<long long, long long>> vertices{
        {0, 0}, {4, 0}, {0, 4}
    };
    const std::vector<std::pair<long long, long long>> points{{1, 1}};
    const CountPointsInTriangles counter(vertices, points);
    for(const auto [first, second, third]:
        std::vector<std::array<std::size_t, 3>>{
            {3, 1, 2}, {0, 3, 2}, {0, 1, 3}
        }){
        bool thrown = false;
        try{
            static_cast<void>(counter.count(first, second, third));
        }catch(const std::out_of_range&){
            thrown = true;
        }
        assert(thrown);
    }
}

int main(){
    test_invalid_indices();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t vertex_count;
    std::cin >> vertex_count;
    std::vector<std::pair<long long, long long>> vertices(vertex_count);
    for(auto& [x, y]: vertices) std::cin >> x >> y;
    std::size_t point_count;
    std::cin >> point_count;
    std::vector<std::pair<long long, long long>> points(point_count);
    for(auto& [x, y]: points) std::cin >> x >> y;

    const CountPointsInTriangles counter(vertices, points);
    std::size_t query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::size_t first;
        std::size_t second;
        std::size_t third;
        std::cin >> first >> second >> third;
        std::cout << counter.count(first, second, third) << '\n';
    }
}
