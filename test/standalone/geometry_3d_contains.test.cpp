// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/contains.hpp"
#include "geometry_3d_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    if(!geometry3d_rejects_invalid_spheres([](const Sphere3& sphere){
        (void)contains(sphere, Point3{0, 0, 0});
    })) return 1;
    return geometry3d_test_main([](){
        const long double translation = 1e3000L;
        const long double tiny = 1e-3000L;
        if(contains(
            {{0, 0, translation}, tiny}, {2 * tiny, 0, translation}
        ) != -1 || contains(
            {{0, 0, translation}, tiny}, {tiny, 0, translation}
        ) != 0) return false;
        return contains(
            Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
            {0.5L, 0.5L, 0}
        ) && !contains(
            Triangle3{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}},
            {0.5L, 0.5L, 1}
        ) && contains(
            Tetrahedron3{
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
            },
            {0.1L, 0.2L, 0.3L}
        ) && contains(Sphere3{{0, 0, 0}, 3}, {1, 1, 1}) == 1 &&
            contains(Sphere3{{0, 0, 0}, 3}, {3, 0, 0}) == 0 &&
            contains(Sphere3{{0, 0, 0}, 3}, {4, 0, 0}) == -1;
    });
}
