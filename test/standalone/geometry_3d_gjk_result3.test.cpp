// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/query/gjk_result3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        GJKResult3 result;
        result.intersects = true;
        result.simplex.push_back({{1, 0, 0}, {2, 0, 0}, {1, 0, 0}});
        return result.intersects && result.simplex.size() == 1
            && geometry3d_api_close(
                result.simplex[0].point_on_first
                    - result.simplex[0].point_on_second,
                result.simplex[0].point
            );
    });
}
