// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circle_point_distance.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_throws<std::overflow_error>([]{
        const long double maximum = std::numeric_limits<long double>::max();
        (void)distance(Circle3{{maximum, 0, 0}, {0, 0, 1}, 0},
            Point3{-maximum, 0, 0});
    })) return 1;
    if(!geometry3d_rejects_invalid_circles([](const Circle3& circle){
        (void)distance(circle, Point3{2, 0, 0});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{0, 0, 0}, {0, 0, 1}, 2};
        return geometry3d_api_close(distance(circle, {2, 0, 0}), 0) &&
            geometry3d_api_close(distance(circle, {0, 0, 3}), std::sqrt(13.0L)) &&
            geometry3d_api_close(distance(circle, {5, 0, 4}), 5);
    });
}
