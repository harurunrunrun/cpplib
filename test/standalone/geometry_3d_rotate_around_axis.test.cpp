// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/rotate_around_axis.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            rotate_around_axis(
                {1, 0, 0}, {0, 0, 2}, GEOMETRY3D_PI / 2
            ),
            {0, 1, 0}
        );
    });
}
