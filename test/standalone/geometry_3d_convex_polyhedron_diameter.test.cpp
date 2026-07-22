// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/convex_polyhedron_diameter.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const ConvexPolyhedron3 cube = geometry3d_test_cube();
        const auto result = convex_polyhedron_diameter(cube);
        const auto bounded_result =
            convex_polyhedron_diameter_aabb_branch_and_bound(cube);
        if(!result || !bounded_result) return false;
        if(result->first != bounded_result->first ||
           result->second != bounded_result->second ||
           result->distance != bounded_result->distance) return false;
        if(!geometry3d_convex_close(result->distance, std::sqrt(12.0L)))
            return false;
        ConvexPolyhedron3 singleton;
        singleton.affine_dimension = 0;
        singleton.vertices = {{1, 2, 3}};
        return !convex_polyhedron_diameter(singleton) &&
            !convex_polyhedron_diameter_aabb_branch_and_bound(singleton);
    });
}
