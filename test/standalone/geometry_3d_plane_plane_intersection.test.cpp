// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/plane_plane_intersection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Line3 line = plane_plane_intersection(
            {{1, 0, 0}, {1, 0, 0}},
            {{0, 2, 0}, {0, 1, 0}}
        );
        return geometry3d_close_value(line.a.x, 1) &&
            geometry3d_close_value(line.a.y, 2) &&
            geometry3d_close_value(line.b.x, 1) &&
            geometry3d_close_value(line.b.y, 2);
    });
}
