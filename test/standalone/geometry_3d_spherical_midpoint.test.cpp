// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/spherical_midpoint.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 expected{std::sqrt(0.5L), std::sqrt(0.5L), 0};
        if(!geometry3d_api_close(spherical_midpoint({3, 0, 0}, {0, 7, 0}), expected)){
            return false;
        }
        bool antipodal_rejected = false;
        try{ (void)spherical_midpoint({1, 0, 0}, {-1, 0, 0}); }
        catch(const std::domain_error&){ antipodal_rejected = true; }
        if(!antipodal_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const long double angle = static_cast<long double>(random() % 999) / 1000.0L
                * GEOMETRY3D_PI;
            const Point3 first{1, 0, 0};
            const Point3 second{std::cos(angle), std::sin(angle), 0};
            const Point3 midpoint = spherical_midpoint(first, second);
            if(!geometry3d_api_close(spherical_central_angle(first, midpoint), angle / 2)
                || !geometry3d_api_close(spherical_central_angle(midpoint, second), angle / 2)){
                return false;
            }
        }
        return true;
    });
}
