// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/scalar/polygon3_signed_area.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{0, 0, 1}, {2, 0, 1}, {2, 2, 1}, {0, 2, 1}};
        return geometry3d_api_close(polygon3_signed_area(polygon, {0, 0, 3}), 4)
            && geometry3d_api_close(polygon3_signed_area(polygon, {0, 0, -1}), -4);
    });
}
