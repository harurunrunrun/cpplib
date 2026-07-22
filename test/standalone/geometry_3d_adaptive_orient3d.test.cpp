// competitive-verifier: STANDALONE

#include <cstdint>
#include <limits>
#include <random>

#include "../../src/algorithm/geometry/3d/predicate/adaptive_orient3d.hpp"
#include "../../src/algorithm/geometry/3d/predicate/exact_orient3d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        for(std::size_t iteration = 0; iteration < rounds * 20; ++iteration){
            std::array<IntegerPoint3, 4> integer_points{};
            std::array<Point3, 4> points{};
            for(std::size_t index = 0; index < points.size(); ++index){
                for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
                    integer_points[index][coordinate] =
                        static_cast<std::int64_t>(random() % 2'000'001) - 1'000'000;
                }
                points[index] = {
                    static_cast<long double>(integer_points[index][0]),
                    static_cast<long double>(integer_points[index][1]),
                    static_cast<long double>(integer_points[index][2]),
                };
            }
            if(adaptive_orient3d(points[0], points[1], points[2], points[3])
                != exact_orient3d(
                    integer_points[0], integer_points[1],
                    integer_points[2], integer_points[3]
                )) return false;
        }
        const long double base = 1.0e18L;
        if(adaptive_orient3d(
            {base, base, base}, {base + 1, base, base},
            {base, base + 1, base}, {base, base, base + 1}
        ) != 1) return false;
        const long double huge = std::numeric_limits<long double>::max() * 0.75L;
        if(adaptive_orient3d(
            {-huge, -huge, -huge}, {huge, -huge, -huge},
            {-huge, huge, -huge}, {-huge, -huge, huge}
        ) != 1) return false;
        return true;
    });
}
