// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sphere_relation.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Sphere3 base{{}, 3};
        return sphere_relation(base, base) == SPHERE_COINCIDENT &&
            sphere_relation(base, {{7, 0, 0}, 3}) == SPHERE_SEPARATE &&
            sphere_relation(base, {{6, 0, 0}, 3}) == SPHERE_EXTERNALLY_TANGENT &&
            sphere_relation(base, {{4, 0, 0}, 3}) == SPHERE_INTERSECT &&
            sphere_relation(base, {{2, 0, 0}, 1}) == SPHERE_INTERNALLY_TANGENT &&
            sphere_relation(base, {{1, 0, 0}, 1}) == SPHERE_INSIDE;
    });
}
