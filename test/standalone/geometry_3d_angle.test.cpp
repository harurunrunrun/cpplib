// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/angle.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            angle({1, 0, 0}, {0, 1, 0}), GEOMETRY3D_PI / 2
        ) && geometry3d_close_value(
            angle({1e3000L, 0, 0}, {1e3000L, 1e3000L, 0}),
            GEOMETRY3D_PI / 4
        ) && geometry3d_close_value(
            angle({1e-3000L, 0, 0}, {1e-3000L, 1e-3000L, 0}),
            GEOMETRY3D_PI / 4
        );
    });
}
