// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/circle_point_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Circle3 circle{{0, 0, 0}, {0, 0, 1}, 2};
        return geometry3d_api_close(distance(circle, {2, 0, 0}), 0) &&
            geometry3d_api_close(distance(circle, {0, 0, 3}), std::sqrt(13.0L)) &&
            geometry3d_api_close(distance(circle, {5, 0, 4}), 5);
    });
}
