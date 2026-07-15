// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/tetrahedron_orientation.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Tetrahedron3 positive{{}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        return tetrahedron_orientation(positive) == 1 &&
            tetrahedron_orientation({positive.a, positive.c, positive.b, positive.d}) == -1 &&
            tetrahedron_orientation({{}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}) == 0;
    });
}
