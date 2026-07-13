// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_sphere_intersection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto circle = sphere_sphere_intersection(
            {{0, 0, 0}, 2}, {{2, 0, 0}, 2}
        );
        return circle &&
            geometry3d_close_point(circle->center, {1, 0, 0}) &&
            geometry3d_close_value(circle->radius, std::sqrt(3.0L));
    });
}
