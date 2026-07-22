// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/area.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return geometry3d_close_value(
            area({{0, 0, 0}, {2, 0, 0}, {0, 2, 0}}), 2
        );
    });
}
