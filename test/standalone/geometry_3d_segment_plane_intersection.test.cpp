// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/segment_plane_intersection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const auto hit = segment_plane_intersection(
            {{0, 0, -1}, {0, 0, 1}},
            {{0, 0, 0}, {0, 0, 1}}
        );
        return hit && geometry3d_close_point(*hit, {0, 0, 0}) &&
            !segment_plane_intersection(
                {{0, 0, 1}, {1, 0, 1}},
                {{0, 0, 0}, {0, 0, 1}}
            );
    });
}
