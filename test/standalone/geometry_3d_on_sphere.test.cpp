// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/on_sphere.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 2};
        if(!on_sphere(sphere, {3, 2, 3}) || on_sphere(sphere, sphere.center)) return false;
        if(!on_sphere(Sphere3{{1, 2, 3}, 0}, {1, 2, 3})) return false;
        try{ (void)on_sphere(Sphere3{{}, -1}, {}); }
        catch(const std::invalid_argument&){ return true; }
        return false;
    });
}
