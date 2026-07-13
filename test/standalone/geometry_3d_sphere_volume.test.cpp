// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/sphere_volume.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            sphere_volume({{0, 0, 0}, 3}),
            36 * GEOMETRY3D_PI
        );
    });
}
