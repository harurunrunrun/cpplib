// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_maximum_area_triangle.hpp"
#include "../../src/algorithm/geometry/2d/result/mixed/convex_polygon_maximum_area_triangle_normalized.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/convex_polygon_maximum_area_triangle_points.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/convex_polygon_maximum_area_triangle_result.hpp"
#include "convex_calipers_test_common.hpp"

namespace{

void self_check(){
    const ConvexPolygonMaximumAreaTriangleResult invalid;
    if(invalid.valid() || invalid.area() != 0.0L){
        throw std::runtime_error("a default result must be invalid");
    }

    const ConvexPolygonMaximumAreaTriangleResult triangle =
        convex_polygon_maximum_area_triangle(
            std::vector<Point>{{0.0L, 0.0L}, {4.0L, 0.0L}, {0.0L, 3.0L}}
        );
    if(!triangle.valid()){
        throw std::runtime_error("a triangle has no maximum-area triangle");
    }
    check_calipers_equal(triangle.doubled_area, 12.0L);
    check_calipers_equal(triangle.area(), 6.0L);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_check();

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        NormalizedConvexPolygon polygon(read_calipers_polygon());
        const ConvexPolygonMaximumAreaTriangleResult result =
            convex_polygon_maximum_area_triangle(polygon);
        const bool expected_valid = polygon.size() >= 3;
        if(result.valid() != expected_valid){
            throw std::runtime_error("invalid maximum-area triangle state");
        }
        if(result.valid()){
            const auto& indices = result.indices;
            if(!(indices[0] < indices[1] && indices[1] < indices[2]
                && indices[2] < polygon.size())){
                throw std::runtime_error("invalid maximum-area triangle indices");
            }
            check_calipers_equal(
                result.doubled_area,
                rotating_calipers_detail::doubled_triangle_area(
                    polygon.vertices()[indices[0]],
                    polygon.vertices()[indices[1]],
                    polygon.vertices()[indices[2]]
                )
            );
        }
        std::cout << static_cast<int>(result.valid()) << ' '
                  << rounded_calipers_value(result.doubled_area) << '\n';
    }
}
