// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/on_segment.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        return on_segment({{0, 0, 0}, {2, 0, 0}}, {1, 0, 0}) &&
            !on_segment({{0, 0, 0}, {2, 0, 0}}, {3, 0, 0}) &&
            on_segment({{1, 2, 3}, {1, 2, 3}}, {1, 2, 3});
    });
}
