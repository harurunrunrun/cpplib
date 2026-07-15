// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/orient_polygon3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 result = orient_polygon3(
            {{0, 0, 1}, {0, 2, 1}, {2, 2, 1}, {2, 0, 1}}, {0, 0, 1}
        );
        return geometry3d_api_close(result.front(), {0, 0, 1})
            && polygon3_signed_area(result, {0, 0, 1}) > 0;
    });
}
