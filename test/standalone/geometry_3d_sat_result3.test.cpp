// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/sat_result3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        SATResult3 result{true, 0, 2, {1, 0, 0}, 15};
        return result.intersects && result.penetration_depth == 2
            && result.tested_axes == 15;
    });
}
