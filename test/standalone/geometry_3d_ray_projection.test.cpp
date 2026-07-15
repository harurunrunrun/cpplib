// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/ray_projection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Ray3 ray{{1, 0, 0}, {3, 0, 0}};
        return geometry3d_api_close(projection(ray, {2, 4, 0}), {2, 0, 0}) &&
            geometry3d_api_close(projection(ray, {-2, 1, 0}), ray.origin);
    });
}
