// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/vector3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        Vector3 value{1, 2, 3};
        return geometry3d_api_close(value + Vector3{2, -2, 1}, {3, 0, 4});
    });
}
