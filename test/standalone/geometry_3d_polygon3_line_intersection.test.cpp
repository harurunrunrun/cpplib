// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3_line_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

Polygon3 concave_polygon(){
    return {
        {-3, -2, 0}, {3, -2, 0}, {3, 2, 0}, {1, 2, 0},
        {1, 0, 0}, {-1, 0, 0}, {-1, 2, 0}, {-3, 2, 0},
    };
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto coplanar = polygon3_line_intersection(
            concave_polygon(), {{-4, 1, 0}, {4, 1, 0}}
        );
        const auto transverse = polygon3_line_intersection(
            concave_polygon(), {{0, -1, -2}, {0, -1, 2}}
        );
        return coplanar.points.empty() && coplanar.segments.size() == 2
            && transverse.points.size() == 1 && transverse.segments.empty();
    });
}
