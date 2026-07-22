// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/convex_polyhedron_surface_area.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_convex_close(
            convex_polyhedron_surface_area(geometry3d_test_cube()), 24.0L
        );
    });
}
