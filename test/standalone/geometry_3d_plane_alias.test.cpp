// competitive-verifier: STANDALONE

#include <type_traits>
#include "../../src/algorithm/geometry/3d/plane.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        Plane plane{{1, 2, 3}, {0, 0, 1}};
        return std::is_same_v<Plane, Plane3> && plane.point == Point3{1, 2, 3};
    });
}
