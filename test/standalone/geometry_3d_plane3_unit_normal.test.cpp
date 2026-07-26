// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/plane3_unit_normal.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            plane3_unit_normal({{0, 0, 0}, {0, 0, 4}}), {0, 0, 1}
        );
    });
}
