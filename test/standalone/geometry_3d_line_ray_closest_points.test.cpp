// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/line_ray_closest_points.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Line3 line{{}, {1, 0, 0}};
        const Ray3 away{{0, 2, 0}, {0, 3, 0}};
        const auto points = closest_points(line, away);
        const auto reversed = closest_points(away, line);
        return geometry3d_api_close(points.first, {}) &&
            geometry3d_api_close(points.second, away.origin) &&
            geometry3d_api_close(reversed.first, away.origin) &&
            geometry3d_api_close(reversed.second, {});
    });
}
