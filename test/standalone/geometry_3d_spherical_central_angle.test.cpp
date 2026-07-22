// competitive-verifier: STANDALONE

#include <limits>

#include "../../src/algorithm/geometry/3d/scalar/spherical_central_angle.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!geometry3d_api_close(spherical_central_angle({1, 0, 0}, {0, 1, 0}), GEOMETRY3D_PI / 2)
            || !geometry3d_api_close(spherical_central_angle({1, 0, 0}, {-1, 0, 0}), GEOMETRY3D_PI)
            || !geometry3d_api_close(spherical_central_angle({1, 2, 3}, {2, 4, 6}), 0.0L)){
            return false;
        }
        bool zero_rejected = false;
        bool non_finite_rejected = false;
        try{ (void)spherical_central_angle({}, {1, 0, 0}); }
        catch(const std::invalid_argument&){ zero_rejected = true; }
        try{
            (void)spherical_central_angle(
                {std::numeric_limits<long double>::infinity(), 0, 0}, {1, 0, 0}
            );
        }catch(const std::invalid_argument&){ non_finite_rejected = true; }
        if(!zero_rejected || !non_finite_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            Point3 first{
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
            };
            Point3 second{
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
            };
            if(first.x == 0 && first.y == 0 && first.z == 0) first.x = 1;
            if(second.x == 0 && second.y == 0 && second.z == 0) second.y = 1;
            const long double angle = spherical_central_angle(first, second);
            if(angle < 0 || angle > GEOMETRY3D_PI
                || !geometry3d_api_close(angle, spherical_central_angle(second, first))
                || !geometry3d_api_close(angle, spherical_central_angle(13 * first, 7 * second))){
                return false;
            }
        }
        return true;
    });
}
