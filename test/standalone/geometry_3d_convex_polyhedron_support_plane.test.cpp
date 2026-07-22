// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/convex_polyhedron_support_plane.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 plane = convex_polyhedron_support_plane(
            geometry3d_test_cube(), {1, 0, 0}
        );
        return plane.point.x == 1.0L && plane.normal.x == 1.0L;
    });
}
