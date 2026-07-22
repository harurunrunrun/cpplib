// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/query/convex_polyhedron_ray_interval.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto interval = convex_polyhedron_ray_interval(
            geometry3d_test_cube(), {{-2, 0, 0}, {-1, 0, 0}}
        );
        const auto away = convex_polyhedron_ray_interval(
            geometry3d_test_cube(), {{-2, 0, 0}, {-3, 0, 0}}
        );
        return interval && geometry3d_convex_close(interval->first, 1.0L)
            && geometry3d_convex_close(interval->second, 3.0L) && !away;
    });
}
