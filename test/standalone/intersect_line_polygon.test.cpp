// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/predicate/line_point_set/intersect_line_polygon.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int count;
    Line line;
    if(!(std::cin >> count >> line.a.x >> line.a.y >> line.b.x >> line.b.y)){
        const std::vector<Point> polygon{
            {0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}
        };
        assert(intersect_line_polygon({{-1, 2}, {4, 2}}, polygon));
        assert(!intersect_line_polygon({{-1, 4}, {4, 4}}, polygon));
        return 0;
    }
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    std::cout << intersect_line_polygon(line, polygon) << '\n';
}
