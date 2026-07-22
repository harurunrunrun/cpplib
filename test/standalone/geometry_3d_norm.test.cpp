// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/norm.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(norm({1, 2, 3}), 14);
    });
}
