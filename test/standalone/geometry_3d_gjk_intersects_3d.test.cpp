// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/gjk_intersects_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!gjk_intersects_3d(
            geometry3d_collision_tetra(), geometry3d_collision_tetra({0.1L, 0, 0})
        )) return false;
        if(gjk_intersects_3d(
            geometry3d_collision_tetra(), geometry3d_collision_tetra({3, 3, 3})
        )) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 center = geometry3d_random_point(random, -3, 3);
            const Point3 half = geometry3d_random_point(random, 0.2L, 1.5L);
            const Point3 other_center = geometry3d_random_point(random, -3, 3);
            const Point3 other_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const bool expected = geometry3d_box_distance(
                center, half, other_center, other_half
            ) <= 1.0e-10L;
            if(gjk_intersects_3d(
                geometry3d_collision_box(center, half),
                geometry3d_collision_box(other_center, other_half)
            ) != expected) return false;
        }
        return true;
    });
}
