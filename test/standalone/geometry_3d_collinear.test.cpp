// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/collinear.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return collinear({1, 1, 1}, {2, 3, 4}, {3, 5, 7}) &&
            !collinear({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
    });
}
