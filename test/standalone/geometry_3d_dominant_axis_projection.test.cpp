// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/dominant_axis3.hpp"
#include "../../src/algorithm/geometry/3d/dominant_axis_from_point3.hpp"
#include "../../src/algorithm/geometry/3d/dominant_axis_projection_to_point.hpp"
#include "../../src/algorithm/geometry/3d/dominant_axis_projection.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const DominantAxis3 axis = dominant_axis({1, -7, 2});
        const Point point = dominant_axis_projection({2, 3, 5}, axis);
        if(axis != DominantAxis3::y ||
            !geometry3d_api_close(point.x, 5) ||
            !geometry3d_api_close(point.y, 2) ||
            dominant_axis({4, -4, 1}) != DominantAxis3::x){
            return false;
        }
        try{
            (void)dominant_axis({});
            return false;
        }catch(const std::invalid_argument&){
        }
        return true;
    });
}
