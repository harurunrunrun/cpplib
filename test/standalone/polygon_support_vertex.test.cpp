// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/index/point_set_point/polygon_support_vertex.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int count;
    Point direction;
    if(!(std::cin >> count >> direction.x >> direction.y)){
        const std::vector<Point> polygon{
            {0, 0}, {4, 0}, {1, 1}, {4, 2}, {0, 2}
        };
        assert(polygon_support_vertex(polygon, {1, 0}) == 1);
        assert(polygon_support_vertex({}, {1, 0}) == POLYGON_SUPPORT_VERTEX_NPOS);
        return 0;
    }
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    const std::size_t answer = polygon_support_vertex(polygon, direction);
    if(answer == POLYGON_SUPPORT_VERTEX_NPOS) std::cout << -1 << '\n';
    else std::cout << answer << '\n';
}
