// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry_3d/abs.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(abs({1, 2, 2}), 3);
    });
}
