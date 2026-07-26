// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/triangle_area_vector.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Triangle3 triangle{{1, 2, 3}, {4, 2, 3}, {1, 6, 3}};
        return geometry3d_api_close(triangle_area_vector(triangle), {0, 0, 6}) &&
            geometry3d_api_close(
                triangle_area_vector({triangle.a, triangle.c, triangle.b}),
                {0, 0, -6}
            );
    });
}
