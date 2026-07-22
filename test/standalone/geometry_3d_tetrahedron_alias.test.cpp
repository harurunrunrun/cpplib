// competitive-verifier: STANDALONE

#include <type_traits>
#include "../../src/algorithm/geometry/3d/core/tetrahedron.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        Tetrahedron tetrahedron{{}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        return std::is_same_v<Tetrahedron, Tetrahedron3> &&
            tetrahedron.d == Point3{0, 0, 1};
    });
}
