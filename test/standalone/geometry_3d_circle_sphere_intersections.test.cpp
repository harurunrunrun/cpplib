// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circle_sphere_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    const Circle3 valid_circle{{0, 0, 0}, {0, 0, 1}, 1};
    const Sphere3 valid_sphere{{0, 0, 0}, 2};
    if(!geometry3d_rejects_invalid_circles([&](const Circle3& circle){
        (void)circle_sphere_intersections(circle, valid_sphere);
    }) || !geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)circle_sphere_intersections(valid_circle, sphere);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{}, {0, 0, 1}, 2};
        const auto points = circle_sphere_intersections(circle, {{2, 0, 0}, 2});
        const auto miss = circle_sphere_intersections(circle, {{}, 1});
        if(points.size() != 2 || !miss.empty()) return false;
        for(const Point3& point: points){
            if(!geometry3d_api_close(point.x, 1) ||
                !geometry3d_api_close(point.z, 0)) return false;
        }
        try{ (void)circle_sphere_intersections(circle, {{}, 2}); }
        catch(const std::domain_error&){ return true; }
        return false;
    });
}
