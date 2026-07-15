// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/minkowski_difference_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const ConvexPolyhedron3 tetra = geometry3d_collision_tetra();
        const ConvexPolyhedron3 difference = minkowski_difference_3d(tetra, tetra);
        if(difference.affine_dimension != 3) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center = geometry3d_random_point(random, -5, 5);
            const Point3 second_center = geometry3d_random_point(random, -5, 5);
            const Point3 first_half = geometry3d_random_point(random, 0.1L, 2);
            const Point3 second_half = geometry3d_random_point(random, 0.1L, 2);
            const ConvexPolyhedron3 result = minkowski_difference_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            Point3 minimum = result.vertices.front();
            Point3 maximum = result.vertices.front();
            for(const Point3& point: result.vertices){
                minimum.x = std::min(minimum.x, point.x);
                minimum.y = std::min(minimum.y, point.y);
                minimum.z = std::min(minimum.z, point.z);
                maximum.x = std::max(maximum.x, point.x);
                maximum.y = std::max(maximum.y, point.y);
                maximum.z = std::max(maximum.z, point.z);
            }
            const Point3 expected_center = first_center - second_center;
            const Point3 expected_half = first_half + second_half;
            if(!geometry3d_api_close(minimum, expected_center - expected_half)
                || !geometry3d_api_close(
                    maximum, expected_center + expected_half
                )) return false;
        }
        return true;
    });
}
