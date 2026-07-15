// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_clip_halfspace.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto half = convex_polyhedron_clip_halfspace(
            geometry3d_test_cube(), {{0, 0, 0}, {0, 0, 1}}
        );
        return geometry3d_convex_close(convex_polyhedron_volume(half), 4.0L)
            && convex_polyhedron_clip_halfspace(
                geometry3d_test_cube(), {{0, 0, -3}, {0, 0, 1}}
            ).vertices.empty();
    });
}
