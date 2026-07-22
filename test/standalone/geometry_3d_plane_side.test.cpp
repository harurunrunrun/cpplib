// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/plane_side.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Plane3 plane{{0, 0, 2}, {0, 0, 3}};
        return plane_side(plane, {0, 0, 4}) == 1 &&
            plane_side(plane, {0, 0, 2}) == 0 &&
            plane_side(plane, {0, 0, 0}) == -1;
    });
}
