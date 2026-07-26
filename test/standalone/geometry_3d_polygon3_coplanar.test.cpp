// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3_coplanar.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        return polygon3_coplanar({{0, 0, 2}, {2, 0, 2}, {2, 2, 2}, {0, 2, 2}})
            && !polygon3_coplanar({{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}});
    });
}
