// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/orthogonal.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return orthogonal({1, 2, 0}, {2, -1, 3}) &&
            !orthogonal({1, 0, 0}, {1, 1, 0});
    });
}
