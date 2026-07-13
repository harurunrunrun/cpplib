// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/parallel.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return parallel({1, 2, 3}, {-2, -4, -6}) &&
            !parallel({1, 0, 0}, {0, 1, 0});
    });
}
