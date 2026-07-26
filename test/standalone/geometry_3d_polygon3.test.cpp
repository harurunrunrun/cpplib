// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/polygon3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
        return polygon.size() == 3 && geometry3d_api_close(polygon[1], {1, 0, 0});
    });
}
