// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/unit.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_point(unit({0, 3, 4}), {0, 0.6L, 0.8L});
    });
}
