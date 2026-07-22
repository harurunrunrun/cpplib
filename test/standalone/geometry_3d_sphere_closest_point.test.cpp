// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/sphere_closest_point.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)closest_point(sphere, Point3{2, 0, 0});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 4};
        if(!geometry3d_api_close(closest_point(sphere, {1, 12, 3}), {1, 6, 3})) return false;
        if(!geometry3d_api_close(closest_point(Sphere3{{1, 2, 3}, 0}, {}), {1, 2, 3})) return false;
        const long double translation = 1e3000L;
        const long double tiny = 1e-3000L;
        const Point3 microscopic = closest_point(
            {{0, 0, translation}, tiny},
            {2 * tiny, 0, translation}
        );
        if(microscopic.x != tiny || microscopic.y != 0.0L ||
            microscopic.z != translation) return false;
        try{ (void)closest_point(sphere, sphere.center); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
