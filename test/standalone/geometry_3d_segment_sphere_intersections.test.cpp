// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/segment_sphere_intersections.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto points = segment_sphere_intersections(
            {{-3, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, 2}
        );
        return points.size() == 1 &&
            geometry3d_close_point(points[0], {-2, 0, 0});
    });
}
