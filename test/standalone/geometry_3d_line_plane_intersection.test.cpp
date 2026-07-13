// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/line_plane_intersection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(
            line_plane_intersection(
                {{1, 2, -4}, {1, 2, 6}},
                {{0, 0, 0}, {0, 0, 1}}
            ),
            {1, 2, 0}
        );
    });
}
