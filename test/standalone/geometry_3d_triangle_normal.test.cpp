// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/triangle_normal.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            triangle_normal({{0, 0, 0}, {2, 0, 0}, {0, 2, 0}}),
            {0, 0, 1}
        );
    });
}
