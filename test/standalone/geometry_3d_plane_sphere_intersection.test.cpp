// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/plane_sphere_intersection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto circle = plane_sphere_intersection(
            {{0, 0, 1}, {0, 0, 1}}, {{0, 0, 0}, 2}
        );
        return circle &&
            geometry3d_close_point(circle->center, {0, 0, 1}) &&
            geometry3d_close_value(circle->radius, std::sqrt(3.0L));
    });
}
