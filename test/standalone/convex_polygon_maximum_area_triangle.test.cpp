// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/convex_polygon_maximum_area_triangle.hpp"
#include "convex_calipers_test_common.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        NormalizedConvexPolygon polygon(read_calipers_polygon());
        const auto result = convex_polygon_maximum_area_triangle(polygon);
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
