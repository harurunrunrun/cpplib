// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_closest_point.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 4};
        if(!geometry3d_api_close(closest_point(sphere, {1, 12, 3}), {1, 6, 3})) return false;
        if(!geometry3d_api_close(closest_point(Sphere3{{1, 2, 3}, 0}, {}), {1, 2, 3})) return false;
        try{ (void)closest_point(sphere, sphere.center); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
