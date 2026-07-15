// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_diameter.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto result = convex_polyhedron_diameter(geometry3d_test_cube());
        return result && geometry3d_convex_close(
            result->distance, std::sqrt(12.0L)
        );
    });
}
