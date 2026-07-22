// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/predicate/intersect_convex_polygons.hpp"

#include <iostream>
#include <vector>

static std::vector<Point> read_polygon(){
    int count;
    std::cin >> count;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    return polygon;
}

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        std::vector<Point> first = read_polygon();
        std::vector<Point> second = read_polygon();
        std::cout << intersect_convex_polygons(
            std::move(first), std::move(second)
        ) << '\n';
    }
}
