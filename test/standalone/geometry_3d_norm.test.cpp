// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/norm.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(norm({1, 2, 3}), 14);
    });
}
