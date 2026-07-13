// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/geometry3d_sign.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_sign(1) == 1 &&
            geometry3d_sign(-1) == -1 &&
            geometry3d_sign(GEOMETRY3D_EPS / 2) == 0;
    });
}
