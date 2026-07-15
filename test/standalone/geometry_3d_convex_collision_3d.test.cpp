// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_collision_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 half{1, 1, 1};
        ConvexCollisionResult3 result = convex_collision_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({3, 0, 0}, half)
        );
        if(result.intersects || !result.converged
            || !geometry3d_api_close(result.distance, 1)) return false;
        result = convex_collision_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({1.5L, 0, 0}, half)
        );
        if(!result.intersects || !result.converged
            || !geometry3d_api_close(result.penetration_depth, 0.5L, 1e-7L)){
            return false;
        }
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center = geometry3d_random_point(random, -4, 4);
            const Point3 second_center = geometry3d_random_point(random, -4, 4);
            const Point3 first_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const Point3 second_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const long double expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            );
            result = convex_collision_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            if(result.intersects != (expected <= 1.0e-10L)) return false;
            if(expected > 0 && !geometry3d_api_close(
                result.distance, expected, 1.0e-8L
            )) return false;
        }
        return true;
    });
}
