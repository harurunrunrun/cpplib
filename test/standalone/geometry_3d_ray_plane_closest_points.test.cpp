// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/ray_plane_closest_points.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Ray3 ray{{0, 0, 2}, {0, 0, 3}};
        const Plane3 plane{{}, {0, 0, 1}};
        const auto points = closest_points(ray, plane);
        return geometry3d_api_close(points.first, ray.origin) &&
            geometry3d_api_close(points.second, {});
    });
}
