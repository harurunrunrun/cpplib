// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/signed_distance.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Plane3 plane{{0, 0, 0}, {0, 0, 2}};
        return geometry3d_close_value(
            signed_distance(plane, {1, 2, -3}), -3
        );
    });
}
