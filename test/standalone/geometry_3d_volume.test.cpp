// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/volume.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            volume({
                {0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {0, 1, 0}
            }),
            1.0L / 6
        );
    });
}
