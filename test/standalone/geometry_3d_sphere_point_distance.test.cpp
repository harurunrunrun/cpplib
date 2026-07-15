// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_point_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 sphere{{1, 2, 3}, 4};
        return geometry3d_api_close(distance(sphere, {5, 2, 3}), 0) &&
            geometry3d_api_close(distance(sphere, sphere.center), 4) &&
            geometry3d_api_close(distance(sphere, {11, 2, 3}), 6);
    });
}
