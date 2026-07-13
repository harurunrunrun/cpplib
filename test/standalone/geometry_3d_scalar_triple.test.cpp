// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/scalar_triple.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            scalar_triple({1, 0, 0}, {0, 1, 0}, {0, 0, 1}), 1
        );
    });
}
