// competitive-verifier: STANDALONE
#include <cmath>

#include "../../src/algorithm/geometry/3d/quaternion3_from_axis_angle.hpp"
#include "../../src/algorithm/geometry/3d/quaternion3_rotate.hpp"
#include "../../src/algorithm/geometry/3d/quaternion3_slerp.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Quaternion3 start = Quaternion3::identity();
        const Quaternion3 finish = quaternion3_from_axis_angle(
            {0, 0, 1}, GEOMETRY3D_PI / 2.0L
        );
        if(!geometry3d_api_close(
                quaternion3_rotate(quaternion3_slerp(start, finish, 0.5L), {1, 0, 0}),
                {std::sqrt(0.5L), std::sqrt(0.5L), 0}
            ) || !geometry3d_api_close(
                quaternion3_rotate(quaternion3_slerp(start, -start, 0.37L), {1, 2, 3}),
                {1, 2, 3}
            )) return false;
        bool parameter_rejected = false;
        try{ (void)quaternion3_slerp(start, finish, -0.1L); }
        catch(const std::invalid_argument&){ parameter_rejected = true; }
        if(!parameter_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 axis{
                static_cast<long double>(random() % 100 + 1),
                static_cast<long double>(random() % 101),
                static_cast<long double>(random() % 101),
            };
            const Quaternion3 end = quaternion3_from_axis_angle(
                axis, static_cast<long double>(random() % 1001) / 1000.0L * GEOMETRY3D_PI
            );
            const long double parameter = static_cast<long double>(random() % 1001) / 1000.0L;
            const Quaternion3 value = quaternion3_slerp(start, end, parameter);
            if(!geometry3d_api_close(value.norm(), 1.0L)
                || !geometry3d_api_close(
                    quaternion3_slerp(start, end, 0.0L).norm(), 1.0L
                ) || !geometry3d_api_close(
                    quaternion3_slerp(start, end, 1.0L).norm(), 1.0L
                )) return false;
        }
        return true;
    });
}
