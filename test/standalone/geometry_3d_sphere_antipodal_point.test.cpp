// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/sphere_antipodal_point.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)sphere_antipodal_point(sphere, Point3{1, 0, 0});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 4};
        if(!geometry3d_api_close(sphere_antipodal_point(sphere, {5, 2, 3}), {-3, 2, 3})) return false;
        if(!geometry3d_api_close(sphere_antipodal_point(Sphere3{{1, 2, 3}, 0}, {1, 2, 3}), {1, 2, 3})) return false;
        try{ (void)sphere_antipodal_point(sphere, sphere.center); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
