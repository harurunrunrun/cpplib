// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/circumcircle.hpp"
#include "geometry_3d_center_radius_extreme_test_common.hpp"
#include "geometry_3d_test_common.hpp"

int main(){
    return geometry3d_test_main([](){
        const Circle3 circle = circumcircle(
            {{0, 0, 0}, {2, 0, 0}, {0, 2, 0}}
        );
        if(!geometry3d_close_point(circle.center, {1, 1, 0})
            || !geometry3d_close_point(circle.normal, {0, 0, 1})
            || !geometry3d_close_value(circle.radius, std::sqrt(2.0L))){
            return false;
        }
        for(const long double scale: {1e3000L, 1e-3000L}){
            const Circle3 extreme = circumcircle(
                geometry3d_extreme_right_triangle(scale)
            );
            if(!geometry3d_extreme_relative_close(extreme.center.x, scale)
                || !geometry3d_extreme_relative_close(extreme.center.y, scale)
                || extreme.center.z != 1e3000L
                || !geometry3d_extreme_relative_close(
                    extreme.radius, std::sqrt(2.0L) * scale
                )
                || extreme.normal.z != 1.0L){
                return false;
            }
        }
        return true;
    });
}
