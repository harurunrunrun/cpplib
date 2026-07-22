// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/dominant_axis_projection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Point point = dominant_axis_projection({2, 3, 5}, DominantAxis3::y);
        return dominant_axis({1, -7, 2}) == DominantAxis3::y
            && geometry3d_api_close(point.x, 5)
            && geometry3d_api_close(point.y, 2);
    });
}
