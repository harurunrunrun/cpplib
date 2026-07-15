// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/triangle_circumradius.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(
            triangle_circumradius({{}, {3, 0, 0}, {0, 4, 0}}), 2.5L
        );
    });
}
