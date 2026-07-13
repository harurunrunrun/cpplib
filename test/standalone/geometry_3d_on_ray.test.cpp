// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/on_ray.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Ray3 ray{{0, 0, 0}, {1, 0, 0}};
        return on_ray(ray, {0, 0, 0}) && on_ray(ray, {7, 0, 0}) &&
            !on_ray(ray, {-1, 0, 0});
    });
}
