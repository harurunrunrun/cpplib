// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/gjk_distance_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center = geometry3d_random_point(random, -5, 5);
            const Point3 second_center = geometry3d_random_point(random, -5, 5);
            const Point3 first_half = geometry3d_random_point(random, 0.1L, 2);
            const Point3 second_half = geometry3d_random_point(random, 0.1L, 2);
            const long double expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            );
            const long double actual = gjk_distance_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            if(!geometry3d_api_close(actual, expected, 1.0e-8L)) return false;
        }
        return true;
    });
}
