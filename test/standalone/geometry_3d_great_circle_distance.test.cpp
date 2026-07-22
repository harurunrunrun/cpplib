// competitive-verifier: STANDALONE
#include <limits>

#include "../../src/algorithm/geometry/3d/scalar/great_circle_distance.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!geometry3d_api_close(
            great_circle_distance({1, 0, 0}, {0, 1, 0}, 4), 2 * GEOMETRY3D_PI
        ) || great_circle_distance({1, 0, 0}, {-1, 0, 0}, 0) != 0.0L) return false;
        bool radius_rejected = false;
        bool overflow_rejected = false;
        try{ (void)great_circle_distance({1, 0, 0}, {0, 1, 0}, -1); }
        catch(const std::invalid_argument&){ radius_rejected = true; }
        try{
            (void)great_circle_distance(
                {1, 0, 0}, {0, 1, 0},
                std::numeric_limits<long double>::max()
            );
        }catch(const std::overflow_error&){ overflow_rejected = true; }
        if(!radius_rejected or !overflow_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const long double angle = static_cast<long double>(random() % 1001) / 1000.0L
                * GEOMETRY3D_PI;
            const long double radius = static_cast<long double>(random() % 1001) / 10.0L;
            const Point3 second{std::cos(angle), std::sin(angle), 0};
            if(!geometry3d_api_close(
                great_circle_distance({1, 0, 0}, second, radius), radius * angle
            )) return false;
        }
        return true;
    });
}
