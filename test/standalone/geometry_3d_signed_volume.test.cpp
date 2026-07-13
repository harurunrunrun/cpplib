// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/signed_volume.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            signed_volume({
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
            }),
            1.0L / 6
        );
    });
}
