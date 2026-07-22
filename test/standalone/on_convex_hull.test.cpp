// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/predicate/on_convex_hull.hpp"

#include <iostream>
#include <vector>

int main(){
    int count, queries;
    std::cin >> count >> queries;
    std::vector<Point> polygon(static_cast<std::size_t>(count));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    const ConvexPolygonQuery prepared(polygon);
    while(queries-- > 0){
        Point point;
        std::cin >> point.x >> point.y;
        std::cout << on_convex_hull(prepared, point) << ' '
                  << on_convex_hull(polygon, point) << '\n';
    }
}
