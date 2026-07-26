// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/on_line.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Line3 line{{0, 0, 0}, {2, 0, 0}};
        return on_line(line, {7, 0, 0}) && !on_line(line, {0, 1, 0});
    });
}
