// competitive-verifier: STANDALONE

#include <cmath>

#include "../../src/algorithm/geometry/3d/shape/quaternion3_from_axis_angle.hpp"
#include "../../src/algorithm/geometry/3d/point/quaternion3_rotate.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

long double squared_length(const Point3& point){
    return point.x * point.x + point.y * point.y + point.z * point.z;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Quaternion3 quarter = quaternion3_from_axis_angle(
            {0, 0, 1}, GEOMETRY3D_PI / 2.0L
        );
        if(!geometry3d_api_close(quaternion3_rotate(quarter, {1, 0, 0}), {0, 1, 0})){
            return false;
        }
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            Point3 axis{
                static_cast<long double>(static_cast<int>(random() % 101) - 50),
                static_cast<long double>(static_cast<int>(random() % 101) - 50),
                static_cast<long double>(static_cast<int>(random() % 101) - 50),
            };
            if(squared_length(axis) == 0.0L) axis.x = 1.0L;
            const long double angle = static_cast<long double>(random() % 2001) / 1000.0L
                * GEOMETRY3D_PI - GEOMETRY3D_PI;
            const Point3 point{
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
            };
            const Quaternion3 rotation = quaternion3_from_axis_angle(axis, angle);
            const Point3 rotated = quaternion3_rotate(rotation, point);
            if(!geometry3d_api_close(squared_length(rotated), squared_length(point))
                || !geometry3d_api_close(
                    quaternion3_rotate(rotation.inverse(), rotated), point
                )) return false;
        }
        bool zero_rejected = false;
        try{ (void)quaternion3_rotate({}, {1, 2, 3}); }
        catch(const std::invalid_argument&){ zero_rejected = true; }
        return zero_rejected;
    });
}
