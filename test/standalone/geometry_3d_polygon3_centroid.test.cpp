// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3_centroid.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return geometry3d_api_close(
            polygon3_centroid({{0, 0, 5}, {4, 0, 5}, {4, 2, 5}, {0, 2, 5}}),
            {2, 1, 5}
        );
    });
}
