// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/line_line_angle.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(line_line_angle(
            Line3{{}, {1, 0, 0}}, Line3{{}, {-1, 0, 0}}
        ), 0) && geometry3d_api_close(line_line_angle(
            Line3{{}, {1, 0, 0}}, Line3{{}, {0, 1, 0}}
        ), GEOMETRY3D_PI / 2);
    });
}
