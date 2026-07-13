// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_intersection_volume.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            sphere_intersection_volume(
                {{0, 0, 0}, 1}, {{0, 0, 0}, 1}
            ),
            4 * GEOMETRY3D_PI / 3
        ) && geometry3d_close_value(
            sphere_intersection_volume(
                {{0, 0, 0}, 1}, {{2, 0, 0}, 1}
            ),
            0
        );
    });
}
