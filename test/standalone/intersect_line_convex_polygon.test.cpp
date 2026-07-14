// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/intersect_line_convex_polygon.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Point> polygon(static_cast<std::size_t>(count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        Line line;
        std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        try{
            const NormalizedConvexPolygon prepared(polygon);
            std::cout << intersect_line_convex_polygon(line, prepared) << ' '
                      << intersect_line_convex_polygon(line, polygon) << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
