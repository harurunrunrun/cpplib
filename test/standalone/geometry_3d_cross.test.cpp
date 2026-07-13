// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/cross.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            cross({1, 0, 0}, {0, 1, 0}), {0, 0, 1}
        );
    });
}
