// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/separating_axis_theorem_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 half{1, 1, 1};
        SATResult3 result = separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({3, 0, 0}, half)
        );
        if(result.intersects || !geometry3d_api_close(result.separation, 1)
            || result.tested_axes < 3) return false;
        result = separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({1.5L, 0, 0}, half)
        );
        if(!result.intersects
            || !geometry3d_api_close(result.penetration_depth, 0.5L)){
            return false;
        }
        if(!separating_axis_theorem_3d(
            geometry3d_collision_tetra(),
            geometry3d_collision_tetra({0.1L, 0.1L, 0.1L})
        ).intersects) return false;
        if(separating_axis_theorem_3d(
            geometry3d_collision_tetra(),
            geometry3d_collision_tetra({3, 3, 3})
        ).intersects) return false;
        const long double quarter_turn = std::acos(-1.0L) / 4.0L;
        if(separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_rotated_z_box(
                {3, 0, 0}, half, quarter_turn
            )
        ).intersects) return false;
        if(!separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_rotated_z_box(
                {2, 0, 0}, half, quarter_turn
            )
        ).intersects) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center = geometry3d_random_point(random, -4, 4);
            const Point3 second_center = geometry3d_random_point(random, -4, 4);
            const Point3 first_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const Point3 second_half = geometry3d_random_point(random, 0.2L, 1.5L);
            result = separating_axis_theorem_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            const bool expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            ) <= 1.0e-10L;
            if(result.intersects != expected) return false;
        }
        return true;
    });
}
