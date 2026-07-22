// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/scalar/sphere_sphere_distance.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_circle_sphere_validation_test_common.hpp"

int main(){
    const Sphere3 valid_sphere{{0, 0, 0}, 1};
    if(!geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_sphere_distance(sphere, valid_sphere);
    }) || !geometry3d_rejects_invalid_spheres([&](const Sphere3& sphere){
        (void)sphere_sphere_distance(valid_sphere, sphere);
    })) return 1;
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Sphere3 unit{{0, 0, 0}, 1};
        if(!geometry3d_api_close(
            sphere_sphere_distance(unit, {{5, 0, 0}, 2}),
            2
        )) return false;
        if(!geometry3d_api_close(distance(unit, {{3, 0, 0}, 2}), 0)){
            return false;
        }
        if(!geometry3d_api_close(distance({{0, 0, 0}, 2}, {{2, 0, 0}, 2}), 0)){
            return false;
        }
        if(!geometry3d_api_close(distance({{0, 0, 0}, 5}, {{1, 0, 0}, 1}), 3)){
            return false;
        }
        if(!geometry3d_api_close(distance({{0, 0, 0}, 5}, {{4, 0, 0}, 1}), 0)){
            return false;
        }
        if(!geometry3d_api_close(distance({{0, 0, 0}, 5}, {{0, 0, 0}, 2}), 3)){
            return false;
        }
        const long double tiny = 1e-3000L;
        const long double microscopic = distance(
            {{0, 0, 1e3000L}, 2 * tiny},
            {{0, 0, 1e3000L}, tiny}
        );
        if(std::abs(microscopic / tiny - 1.0L) > 1e-12L){
            return false;
        }
        if(!geometry3d_api_close(distance({{0, 0, 0}, 0}, {{0, 0, 0}, 0}), 0)){
            return false;
        }

        const long double maximum = std::numeric_limits<long double>::max();
        const Sphere3 huge_left{{maximum * 0.75L, 0, 0}, maximum * 0.5L};
        const Sphere3 huge_right{{-maximum * 0.75L, 0, 0}, maximum * 0.5L};
        const long double huge_gap = distance(huge_left, huge_right);
        if(!std::isfinite(huge_gap) ||
           !geometry3d_api_close(huge_gap, maximum * 0.5L, 1e-15L)){
            return false;
        }
        if(distance(
            {{maximum * 0.75L, 0, 0}, maximum * 0.75L},
            {{-maximum * 0.75L, 0, 0}, maximum * 0.75L}
        ) != 0.0L) return false;

        bool threw = false;
        try{
            (void)distance(unit, {{0, 0, 0}, -1});
        }catch(const std::invalid_argument&){
            threw = true;
        }
        if(!threw) return false;
        threw = false;
        try{
            (void)distance(
                unit,
                {{std::numeric_limits<long double>::infinity(), 0, 0}, 1}
            );
        }catch(const std::invalid_argument&){
            threw = true;
        }
        if(!threw) return false;
        threw = false;
        try{
            (void)distance(
                {{maximum * 0.75L, 0, 0}, 0},
                {{-maximum * 0.75L, 0, 0}, 0}
            );
        }catch(const std::overflow_error&){
            threw = true;
        }
        if(!threw) return false;

        std::uniform_real_distribution<long double> coordinate(-1000.0L, 1000.0L);
        std::uniform_real_distribution<long double> radius(0.0L, 500.0L);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Sphere3 first{
                {coordinate(random), coordinate(random), coordinate(random)},
                radius(random),
            };
            const Sphere3 second{
                {coordinate(random), coordinate(random), coordinate(random)},
                radius(random),
            };
            const long double center_distance = std::hypot(
                first.center.x - second.center.x,
                first.center.y - second.center.y,
                first.center.z - second.center.z
            );
            const long double expected = std::max({
                0.0L,
                center_distance - first.radius - second.radius,
                std::abs(first.radius - second.radius) - center_distance,
            });
            if(!geometry3d_api_close(distance(first, second), expected, 1e-12L)){
                return false;
            }
        }
        return true;
    });
}
