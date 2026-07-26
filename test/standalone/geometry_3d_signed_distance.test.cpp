// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/signed_distance.hpp"
#include "geometry_3d_linear_closest_extreme_test_common.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Plane3 plane{{0, 0, 0}, {0, 0, 2}};
        return geometry3d_close_value(
            signed_distance(plane, {1, 2, -3}), -3
        ) && geometry3d_linear_test_close(
            signed_distance(
                {{0, 0, GEOMETRY3D_LINEAR_TEST_TRANSLATION},
                 {0, 1e3000L, 0}},
                {0, GEOMETRY3D_LINEAR_TEST_LOCAL,
                 GEOMETRY3D_LINEAR_TEST_TRANSLATION}
            ),
            GEOMETRY3D_LINEAR_TEST_LOCAL
        );
    });
}
