// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circle_closest_point.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{1, 2, 3}, {0, 0, 2}, 4};
        if(!geometry3d_api_close(closest_point(circle, {11, 2, 8}), {5, 2, 3})) return false;
        if(!geometry3d_api_close(closest_point(Circle3{{1, 2, 3}, {0, 1, 0}, 0}, {}), {1, 2, 3})) return false;
        try{ (void)closest_point(circle, circle.center + Point3{0, 0, 5}); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
