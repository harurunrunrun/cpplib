// competitive-verifier: STANDALONE

#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/principal_axes_3d.hpp"
#include "../../src/algorithm/geometry/3d/dot.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const PrincipalAxes3 line = principal_axes_3d({
            {-2, 4, 7}, {0, 4, 7}, {2, 4, 7}
        });
        if(!geometry3d_api_close(line.centroid, {0, 4, 7}) ||
           std::abs(dot(line.axes[0], {1, 0, 0})) < 1 - 1e-9L ||
           !geometry3d_api_close(line.variances[0], 8.0L / 3) ||
           !geometry3d_api_close(line.variances[1], 0)) return false;
        const PrincipalAxes3 point = principal_axes_3d({{3, -2, 5}, {3, -2, 5}});
        if(!geometry3d_api_close(point.centroid, {3, -2, 5}) ||
           point.variances[0] != 0) return false;
        bool empty_threw = false;
        try{
            static_cast<void>(principal_axes_3d({}));
        }catch(const std::invalid_argument&){ empty_threw = true; }
        if(!empty_threw) return false;

        const long double maximum = std::numeric_limits<long double>::max();
        bool variance_overflow_threw = false;
        try{
            static_cast<void>(principal_axes_3d({
                {-maximum, 0, 0}, {maximum, 0, 0}
            }));
        }catch(const std::overflow_error&){ variance_overflow_threw = true; }
        if(!variance_overflow_threw) return false;

        const Point3 expected_direction{2, -3, 1};
        std::uniform_real_distribution<long double> parameter(-10, 10);
        std::normal_distribution<long double> noise(0, 1e-4L);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            for(int index = 0; index < 30; ++index){
                const long double t = parameter(random);
                points.push_back({
                    5 + 2 * t + noise(random),
                    -7 - 3 * t + noise(random),
                    4 + t + noise(random),
                });
            }
            const PrincipalAxes3 actual = principal_axes_3d(points);
            const long double alignment = std::abs(dot(actual.axes[0], expected_direction)) /
                std::hypot(expected_direction.x, expected_direction.y,
                           expected_direction.z);
            if(alignment < 0.999L || actual.variances[0] < actual.variances[1] ||
               actual.variances[1] < actual.variances[2]) return false;
        }
        return true;
    });
}
