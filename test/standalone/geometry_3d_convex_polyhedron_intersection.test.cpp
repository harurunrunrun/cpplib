// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_intersection.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto overlap = convex_polyhedron_intersection(
            geometry3d_test_cube(), geometry3d_test_cube({1, 0, 0})
        );
        return geometry3d_convex_close(
            convex_polyhedron_volume(overlap), 4.0L
        );
    });
}
