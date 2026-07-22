// competitive-verifier: STANDALONE

#include <cmath>
#include <limits>

#include "../../src/algorithm/geometry/3d/shape/quaternion3_from_axis_angle.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Quaternion3 quarter = quaternion3_from_axis_angle(
            {0, 0, 7}, GEOMETRY3D_PI / 2.0L
        );
        if(!geometry3d_api_close(quarter.w, std::sqrt(0.5L))
            || !geometry3d_api_close(quarter.x, 0.0L)
            || !geometry3d_api_close(quarter.y, 0.0L)
            || !geometry3d_api_close(quarter.z, std::sqrt(0.5L))) return false;
        bool zero_rejected = false;
        bool non_finite_rejected = false;
        try{ (void)quaternion3_from_axis_angle({0, 0, 0}, 1); }
        catch(const std::invalid_argument&){ zero_rejected = true; }
        try{
            (void)quaternion3_from_axis_angle(
                {1, 0, 0}, std::numeric_limits<long double>::infinity()
            );
        }catch(const std::invalid_argument&){ non_finite_rejected = true; }
        if(!zero_rejected || !non_finite_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 axis{
                static_cast<long double>(random() % 100 + 1),
                static_cast<long double>(random() % 101),
                static_cast<long double>(random() % 101),
            };
            const long double angle = static_cast<long double>(random() % 2001) / 1000.0L
                * GEOMETRY3D_PI - GEOMETRY3D_PI;
            const Quaternion3 value = quaternion3_from_axis_angle(axis, angle);
            if(!geometry3d_api_close(value.norm(), 1.0L)
                || !geometry3d_api_close(value.w, std::cos(angle / 2.0L))) return false;
        }
        return true;
    });
}
