// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_cap_height.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_throws<std::overflow_error>([]{
        const long double maximum = std::numeric_limits<long double>::max();
        (void)sphere_cap_height(
            {{0, 0, 0}, maximum}, {{0, 0, -maximum}, {0, 0, 1}}, 1
        );
    })) return 1;
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)sphere_cap_height(sphere, Plane3{{0, 0, 0}, {0, 0, 1}}, 1);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{0, 0, 1}, 3};
        const Plane3 plane{{0, 0, 0}, {0, 0, 2}};
        if(!geometry3d_api_close(sphere_cap_height(sphere, plane, 1), 4) ||
            !geometry3d_api_close(sphere_cap_height(sphere, plane, -1), 2)) return false;
        const Plane3 above{{0, 0, 10}, {0, 0, 1}};
        if(!geometry3d_api_close(sphere_cap_height(sphere, above, 1), 0) ||
            !geometry3d_api_close(sphere_cap_height(sphere, above, -1), 6)) return false;
        try{ (void)sphere_cap_height(sphere, plane, 0); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
