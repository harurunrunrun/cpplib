// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_projection.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)projection(sphere, Point3{2, 0, 0});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 4};
        if(!geometry3d_api_close(projection(sphere, {11, 2, 3}), {5, 2, 3})) return false;
        if(!geometry3d_api_close(projection(Sphere3{{1, 2, 3}, 0}, {}), {1, 2, 3})) return false;
        try{ (void)projection(sphere, sphere.center); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
