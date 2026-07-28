// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/scalar/polygon/convex_polygon_minimum_width_normalized.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point_set/convex_polygon_minimum_width_points.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_minimum_width.hpp"
#include "geometry_2d_test_common.hpp"

#include <vector>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Point> polygon(static_cast<std::size_t>(count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        const long double from_points = convex_polygon_minimum_width(polygon);
        const NormalizedConvexPolygon normalized(polygon);
        const long double from_normalized =
            convex_polygon_minimum_width(normalized);
        if(from_points != from_normalized){
            throw std::runtime_error("minimum-width overload mismatch");
        }
        geometry_test_print_real(from_points);
    }
}
