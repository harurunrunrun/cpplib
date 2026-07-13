// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/coplanar.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return coplanar(
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {2, 3, 0}
        ) && !coplanar(
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
        );
    });
}
