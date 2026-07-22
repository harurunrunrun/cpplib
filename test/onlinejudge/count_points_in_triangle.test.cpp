// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/count_points_in_triangle

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/query/count_points_in_triangles.hpp"

int main(){
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
