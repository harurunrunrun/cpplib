// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/ray_plane_intersection.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Plane3 plane{{0, 0, 0}, {0, 0, 1}};
        const auto hit = ray_plane_intersection(
            {{0, 0, -2}, {0, 0, 1}}, plane
        );
        return hit && geometry3d_close_point(*hit, {0, 0, 0}) &&
            !ray_plane_intersection({{0, 0, 1}, {0, 0, 2}}, plane);
    });
}
