// competitive-verifier: STANDALONE

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/deduplicate_points_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool exact_same(const Point3& left, const Point3& right){
    return left.x == right.x && left.y == right.y && left.z == right.z;
}

bool same_sequence(
    const std::vector<Point3>& left,
    const std::vector<Point3>& right
){
    if(left.size() != right.size()) return false;
    for(std::size_t index = 0; index < left.size(); ++index){
        if(!exact_same(left[index], right[index])) return false;
    }
    return true;
}

std::vector<Point3> brute_deduplicate(
    const std::vector<Point3>& points,
    long double relative_tolerance,
    long double absolute_tolerance
){
    std::vector<Point3> result;
    for(const Point3& point: points){
        bool duplicate = false;
        for(const Point3& representative: result){
            if(relative_equal(
                    point.x, representative.x,
                    relative_tolerance, absolute_tolerance
                ) && relative_equal(
                    point.y, representative.y,
                    relative_tolerance, absolute_tolerance
                ) && relative_equal(
                    point.z, representative.z,
                    relative_tolerance, absolute_tolerance
                )){
                duplicate = true;
                break;
            }
        }
        if(!duplicate) result.push_back(point);
    }
    return result;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](
        std::mt19937_64& random, std::size_t rounds
    ){
        const std::vector<Point3> result = deduplicate_points_3d({
            {0, 0, 0}, {5.0e-11L, 0, 0}, {2.0e-10L, 0, 0},
            {1, 2, 3}, {1, 2, 3}
        });
        const std::vector<Point3> expected{
            {0, 0, 0}, {2.0e-10L, 0, 0}, {1, 2, 3}
        };
        if(!same_sequence(result, expected)) return false;

        const std::vector<Point3> boundary_points{
            {0.0L, 0.0L, 0.0L},
            {1.0L, 0.0L, 0.0L},
            {1.5L, 0.0L, 0.0L},
            {2.01L, 0.0L, 0.0L},
            {-1.0L, 0.0L, 0.0L},
            {-2.01L, 0.0L, 0.0L},
            {1.5L, 1.0L, 1.0L},
            {1.5L, 2.01L, 1.0L},
        };
        if(!same_sequence(
            deduplicate_points_3d(boundary_points, 0.0L, 1.0L),
            brute_deduplicate(boundary_points, 0.0L, 1.0L)
        )) return false;

        const auto signed_zero = deduplicate_points_3d({
            {-0.0L, 0.0L, 0.0L}, {0.0L, 0.0L, 0.0L}
        }, 0.0L, 0.0L);
        if(signed_zero.size() != 1 || !std::signbit(signed_zero[0].x)){
            return false;
        }

        const auto relative = deduplicate_points_3d({
            {1.0e12L, 0, 0}, {1.0e12L + 50.0L, 0, 0}
        }, 1.0e-10L, 0.0L);
        if(relative.size() != 1) return false;

        constexpr std::array<long double, 5> tolerances{
            0.0L, 0.125L, 0.25L, 0.75L, 2.0L
        };
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t size = 1 + random() % 48;
            points.reserve(size);
            for(std::size_t index = 0; index < size; ++index){
                if(!points.empty() && random() % 7 == 0){
                    points.push_back(points[random() % points.size()]);
                    continue;
                }
                const auto coordinate = [&]{
                    const long long value =
                        static_cast<long long>(random() % 161) - 80;
                    return static_cast<long double>(value) / 8.0L;
                };
                points.push_back({coordinate(), coordinate(), coordinate()});
            }
            const long double tolerance =
                tolerances[random() % tolerances.size()];
            if(!same_sequence(
                deduplicate_points_3d(points, 0.0L, tolerance),
                brute_deduplicate(points, 0.0L, tolerance)
            )) return false;
        }

        const long double maximum = std::numeric_limits<long double>::max();
        const auto maximum_width = deduplicate_points_3d({
            {maximum, 0, 0}, {-maximum, 0, 0}, {0, 0, 0}
        }, 0.0L, maximum);
        if(maximum_width.size() != 2
            || maximum_width[0].x != maximum
            || maximum_width[1].x != -maximum) return false;

        long double tiny = std::numeric_limits<long double>::denorm_min();
        if(tiny == 0.0L) tiny = std::numeric_limits<long double>::min();
        const std::vector<Point3> tiny_boundary{
            {0, 0, 0}, {tiny, 0, 0}, {2 * tiny, 0, 0}, {4 * tiny, 0, 0}
        };
        if(!same_sequence(
            deduplicate_points_3d(tiny_boundary, 0.0L, tiny),
            brute_deduplicate(tiny_boundary, 0.0L, tiny)
        )) return false;
        const auto extreme_cells = deduplicate_points_3d({
            {maximum, maximum, maximum},
            {maximum, maximum, maximum},
            {-maximum, -maximum, -maximum},
        }, 0.0L, tiny);
        if(extreme_cells.size() != 2) return false;

        if(rounds == 32){
            std::vector<Point3> many;
            constexpr std::size_t unique_count = 12000;
            many.reserve(2 * unique_count);
            for(std::size_t index = 0; index < unique_count; ++index){
                const Point3 point{
                    static_cast<long double>(index),
                    static_cast<long double>(index % 97),
                    -static_cast<long double>(index),
                };
                many.push_back(point);
                many.push_back(point);
            }
            const auto unique = deduplicate_points_3d(
                many, 0.0L, 0.0L
            );
            if(unique.size() != unique_count) return false;
            for(std::size_t index = 0; index < unique_count; ++index){
                if(!exact_same(unique[index], many[2 * index])) return false;
            }
        }

        try{
            static_cast<void>(deduplicate_points_3d({{
                0, std::numeric_limits<long double>::quiet_NaN(), 0
            }}));
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(deduplicate_points_3d(
                {}, -1.0L, 0.0L
            ));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
