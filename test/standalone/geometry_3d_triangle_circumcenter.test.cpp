// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/triangle_circumcenter.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Triangle3 triangle{{1, 2, 5}, {5, 2, 5}, {1, 8, 5}};
        return geometry3d_api_close(triangle_circumcenter(triangle), {3, 5, 5});
    });
}
