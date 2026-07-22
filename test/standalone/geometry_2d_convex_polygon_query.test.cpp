// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/query/convex_polygon_query.hpp"
#include "geometry_2d_test_common.hpp"

#include <vector>

int main(){
    geometry_test_setup_output();
    int cases;
    std::cin >> cases;
    while(cases-- > 0){
        int size, queries;
        std::cin >> size >> queries;
        std::vector<Point> polygon(static_cast<std::size_t>(size));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        const ConvexPolygonQuery index(polygon);
        std::cout << index.size() << '\n';
        for(const Point& point: index.vertices()) geometry_test_print_point(point);
        while(queries-- > 0){
            Point point;
            std::cin >> point.x >> point.y;
            std::cout << index.contains(point) << '\n';
        }
    }
}
