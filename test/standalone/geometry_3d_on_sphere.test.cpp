// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/on_sphere.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)on_sphere(sphere, Point3{1, 0, 0});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 2};
        if(!on_sphere(sphere, {3, 2, 3}) || on_sphere(sphere, sphere.center)) return false;
        if(!on_sphere(Sphere3{{1, 2, 3}, 0}, {1, 2, 3})) return false;
        const long double translation = 1e3000L;
        const long double tiny = 1e-3000L;
        if(!on_sphere(
            {{0, 0, translation}, tiny}, {tiny, 0, translation}
        )) return false;
        try{ (void)on_sphere(Sphere3{{}, -1}, {}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
