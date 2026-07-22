// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/convex_polyhedron_sphere_intersects.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)convex_polyhedron_sphere_intersects(geometry3d_test_cube(), sphere);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const auto cube = geometry3d_test_cube();
        if(!convex_polyhedron_sphere_intersects(cube, {{2, 0, 0}, 1})
            || convex_polyhedron_sphere_intersects(cube, {{3, 0, 0}, 1})){
            return false;
        }

        const long double maximum =
            std::numeric_limits<long double>::max();
        ConvexPolyhedron3 far_cube = cube;
        for(Point3& vertex: far_cube.vertices){
            vertex = {
                maximum * (0.5L + 0.25L * vertex.x),
                maximum * (0.5L + 0.25L * vertex.y),
                maximum * (0.5L + 0.25L * vertex.z),
            };
        }
        return !convex_polyhedron_sphere_intersects(
            far_cube, {{-maximum * 0.75L, 0, 0}, 1}
        );
    });
}
