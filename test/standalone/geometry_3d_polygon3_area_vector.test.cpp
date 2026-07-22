// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/polygon3_area_vector.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(
            polygon3_area_vector({{0, 0, 1}, {2, 0, 1}, {2, 2, 1}, {0, 2, 1}}),
            {0, 0, 4}
        );
    });
}
