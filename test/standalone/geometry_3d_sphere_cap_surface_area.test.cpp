// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/sphere_cap_surface_area.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            sphere_cap_surface_area({{0, 0, 0}, 3}, 1),
            6 * GEOMETRY3D_PI
        );
    });
}
