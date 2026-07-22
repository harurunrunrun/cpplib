// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point/spherical_slerp.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!geometry3d_api_close(
            spherical_slerp({1, 0, 0}, {0, 1, 0}, 0.5L),
            {std::sqrt(0.5L), std::sqrt(0.5L), 0}
        )) return false;
        bool antipodal_rejected = false;
        bool parameter_rejected = false;
        try{ (void)spherical_slerp({1, 0, 0}, {-1, 0, 0}, 0.5L); }
        catch(const std::domain_error&){ antipodal_rejected = true; }
        try{ (void)spherical_slerp({1, 0, 0}, {0, 1, 0}, 2); }
        catch(const std::invalid_argument&){ parameter_rejected = true; }
        if(!antipodal_rejected || !parameter_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const long double angle = static_cast<long double>(random() % 999) / 1000.0L
                * GEOMETRY3D_PI;
            const long double parameter = static_cast<long double>(random() % 1001) / 1000.0L;
            const Point3 first{1, 0, 0};
            const Point3 second{std::cos(angle), std::sin(angle), 0};
            const Point3 value = spherical_slerp(first, second, parameter);
            if(!geometry3d_api_close(
                    spherical_central_angle(first, value), parameter * angle
                ) || !geometry3d_api_close(
                    spherical_central_angle(value, second), (1 - parameter) * angle
                )) return false;
        }
        return true;
    });
}
