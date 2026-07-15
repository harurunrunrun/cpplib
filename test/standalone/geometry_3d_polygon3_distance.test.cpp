// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{0, 0, 0}, {4, 0, 0}, {4, 2, 0}, {0, 2, 0}};
        return geometry3d_api_close(polygon3_distance(polygon, {2, 1, 3}), 3)
            && geometry3d_api_close(polygon3_distance(polygon, {5, 1, 0}), 1);
    });
}
