// competitive-verifier: STANDALONE

#include <array>
#include <limits>
#include <cstdint>
#include <random>
#include <stdexcept>

#include "../../src/algorithm/geometry/3d/predicate/adaptive_insphere.hpp"
#include "../../src/algorithm/geometry/3d/predicate/exact_insphere.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        std::size_t checked = 0;
        for(std::size_t iteration = 0; iteration < rounds * 25; ++iteration){
            std::array<IntegerPoint3, 5> integer_points{};
            std::array<Point3, 5> points{};
            for(std::size_t index = 0; index < points.size(); ++index){
                for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
                    integer_points[index][coordinate] =
                        static_cast<std::int64_t>(random() % 101) - 50;
                }
                points[index] = {
                    static_cast<long double>(integer_points[index][0]),
                    static_cast<long double>(integer_points[index][1]),
                    static_cast<long double>(integer_points[index][2]),
                };
            }
            if(exact_orient3d(
                integer_points[0], integer_points[1],
                integer_points[2], integer_points[3]
            ) == 0) continue;
            const int expected = exact_insphere(
                integer_points[0], integer_points[1], integer_points[2],
                integer_points[3], integer_points[4]
            );
            const int actual = adaptive_insphere(
                points[0], points[1], points[2], points[3], points[4]
            );
            if(actual != expected) return false;
            ++checked;
        }
        if(checked < rounds * 20) return false;
        try{
            static_cast<void>(adaptive_insphere(
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}
            ));
            return false;
        }catch(const std::invalid_argument&){}
        const long double huge = std::numeric_limits<long double>::max() * 0.75L;
        const Point3 first{huge, huge, huge};
        const Point3 second{-huge, -huge, huge};
        const Point3 third{-huge, huge, -huge};
        const Point3 fourth{huge, -huge, -huge};
        if(adaptive_insphere(first, second, third, fourth, {}) != 1
            || adaptive_insphere(
                first, second, third, fourth, {-huge, -huge, -huge}
            ) != 0) return false;
        return true;
    });
}
