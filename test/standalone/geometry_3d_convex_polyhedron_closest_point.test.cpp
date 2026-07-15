// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_closest_point.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto cube = geometry3d_test_cube();
        return geometry3d_convex_close(
            convex_polyhedron_closest_point(cube, {3, 0.5L, -0.25L}),
            {1, 0.5L, -0.25L}
        ) && geometry3d_convex_close(
            convex_polyhedron_closest_point(cube, {0, 0, 0}), {0, 0, 0}
        );
    });
}
