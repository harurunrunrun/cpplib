// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/contains.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
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
