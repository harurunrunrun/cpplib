// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/dot.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            dot({1, 2, 3}, {4, -5, 6}), 12
        );
    });
}
