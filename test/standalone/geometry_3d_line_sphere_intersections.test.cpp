// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/line_sphere_intersections.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto points = line_sphere_intersections(
            {{-3, 0, 0}, {3, 0, 0}}, {{0, 0, 0}, 2}
        );
        return points.size() == 2 &&
            geometry3d_close_point(points[0], {-2, 0, 0}) &&
            geometry3d_close_point(points[1], {2, 0, 0});
    });
}
