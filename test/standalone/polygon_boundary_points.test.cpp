// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>
#include "../../src/algorithm/geometry/polygon_boundary_points.hpp"

void self_test(){
    std::vector<Point> polygon = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    std::vector<Point> points = {
        {0, 0},
        {2, 0},
        {4, 2},
        {1, 1},
        {0, 3},
        {2, 4},
    };
    auto result = enumerate_points_on_polygon_boundary(polygon, points);
    std::vector<int> expected = {0, 1, 2, 5, 4};
    assert(result == expected);
    auto duplicated = enumerate_points_on_polygon_boundary(polygon, points, false);
    std::vector<int> expected_duplicated = {0, 1, 2, 5, 4, 0};
    assert(duplicated == expected_duplicated);
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<Point> polygon(static_cast<std::size_t>(n));
    for(auto& p: polygon) std::cin >> p.x >> p.y;
    std::vector<Point> points(static_cast<std::size_t>(m));
    for(auto& p: points) std::cin >> p.x >> p.y;
    auto result = enumerate_points_on_polygon_boundary(polygon, points);
    std::cout << result.size();
    for(int index: result) std::cout << ' ' << index;
    std::cout << '\n';
}
