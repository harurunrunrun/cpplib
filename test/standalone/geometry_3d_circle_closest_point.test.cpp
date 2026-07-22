// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/circle_closest_point.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_circles([](const Circle3& circle){
        (void)closest_point(circle, Point3{2, 0, 0});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{1, 2, 3}, {0, 0, 2}, 4};
        if(!geometry3d_api_close(closest_point(circle, {11, 2, 8}), {5, 2, 3})) return false;
        if(!geometry3d_api_close(closest_point(Circle3{{1, 2, 3}, {0, 1, 0}, 0}, {}), {1, 2, 3})) return false;
        try{ (void)closest_point(circle, circle.center + Point3{0, 0, 5}); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
