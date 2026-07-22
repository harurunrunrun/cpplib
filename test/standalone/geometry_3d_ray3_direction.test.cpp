// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/ray3_direction.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            ray3_direction({{1, 2, 3}, {4, 6, 8}}), {3, 4, 5}
        );
    });
}
