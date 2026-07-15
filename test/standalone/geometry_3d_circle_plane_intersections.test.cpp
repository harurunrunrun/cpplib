// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circle_plane_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_circles([](const Circle3& circle){
        (void)circle_plane_intersections(circle, Plane3{{0, 0, 0}, {1, 0, 0}});
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{}, {0, 0, 1}, 2};
        const auto secant = circle_plane_intersections(circle, {{}, {1, 0, 0}});
        const auto miss = circle_plane_intersections(circle, {{0, 0, 1}, {0, 0, 1}});
        if(secant.size() != 2 || miss.size() != 0) return false;
        if(!geometry3d_api_close(abs(secant[0]), 2) ||
            !geometry3d_api_close(abs(secant[1]), 2)) return false;
        try{ (void)circle_plane_intersections(circle, {{}, {0, 0, -1}}); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
