// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_point_distance.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_throws<std::overflow_error>([]{
        const long double maximum = std::numeric_limits<long double>::max();
        (void)distance(Sphere3{{maximum, 0, 0}, 0}, Point3{-maximum, 0, 0});
    })) return 1;
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)distance(sphere, Point3{2, 0, 0});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 4};
        const long double translation = 1e3000L;
        const long double tiny = 1e-3000L;
        if(std::abs(
            distance(
                {{0, 0, translation}, tiny},
                {2 * tiny, 0, translation}
            ) / tiny - 1.0L
        ) > 1e-12L) return false;
        return geometry3d_api_close(distance(sphere, {5, 2, 3}), 0) &&
            geometry3d_api_close(distance(sphere, sphere.center), 4) &&
            geometry3d_api_close(distance(sphere, {11, 2, 3}), 6);
    });
}
