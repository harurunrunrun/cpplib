// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3_ray_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{
            {-3, -2, 0}, {3, -2, 0}, {3, 2, 0}, {1, 2, 0},
            {1, 0, 0}, {-1, 0, 0}, {-1, 2, 0}, {-3, 2, 0},
        };
        const auto result = polygon3_ray_intersection(
            polygon, {{0, 1, 0}, {1, 1, 0}}
        );
        return result.points.empty() && result.segments.size() == 1
            && result.segments.front().a.x == 1.0L
            && result.segments.front().b.x == 3.0L;
    });
}
