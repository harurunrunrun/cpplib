// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
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

bool coordinate_intervals_are_exact(
    std::vector<long double> values,
    long double fixed,
    long double relative_tolerance,
    long double absolute_tolerance
){
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    const auto intervals =
        deduplicate_points_3d_detail::coordinate_intervals(
            values, fixed, relative_tolerance, absolute_tolerance
        );
    for(std::size_t index = 0; index < values.size(); ++index){
        bool contained = false;
        for(const auto interval: intervals){
            if(interval.first <= index && index < interval.second){
                contained = true;
            }
        }
        if(contained != relative_equal(
            fixed, values[index],
            relative_tolerance, absolute_tolerance
        )){
            return false;
        }
    }
    return true;
}

long double random_finite_coordinate(std::mt19937_64& random){
    const long double mantissa =
        static_cast<long double>(1 + random() % 1024) / 1024.0L;
    const int exponent = static_cast<int>(random() % 2001) - 1000;
    return std::ldexp(random() % 2 == 0 ? mantissa : -mantissa, exponent);
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
        using namespace deduplicate_points_3d_detail;
        const CellCoordinate large_denominator =
            (CellCoordinate(1) << 80) + 3;
        const CellCoordinate large_numerator =
            large_denominator * 17 + 5;
        const ExactDyadic large_width{large_denominator, 0};
        if(floor_ratio({large_numerator, 0}, large_width) != 17){
            return false;
        }
        if(floor_ratio({-large_numerator, 0}, large_width) != -18){
            return false;
        }


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

        const std::vector<Point3> relative_nontransitive{
            {100.0L, 0.0L, 0.0L},
            {110.0L, 0.0L, 0.0L},
            {121.0L, 0.0L, 0.0L},
        };
        if(!same_sequence(
            deduplicate_points_3d_relative_range_tree(
                relative_nontransitive, 0.1L, 0.0L
            ),
            brute_deduplicate(relative_nontransitive, 0.1L, 0.0L)
        )) return false;

        const std::vector<Point3> cartesian_false_positive{
            {0.0L, 100.0L, 100.0L},
            {100.0L, 0.0L, 100.0L},
            {100.0L, 100.0L, 0.0L},
            {0.0L, 0.0L, 0.0L},
        };
        const auto cartesian_result =
            deduplicate_points_3d_relative_range_tree(
                cartesian_false_positive, 0.001L, 0.1L
            );
        if(!same_sequence(cartesian_result, cartesian_false_positive)){
            return false;
        }

        const long double below_six =
            std::nextafter(6.0L, -std::numeric_limits<long double>::infinity());
        const long double above_six =
            std::nextafter(6.0L, std::numeric_limits<long double>::infinity());
        const long double upper_quarter = 8.0L / 0.75L;
        const long double below_upper_quarter = std::nextafter(
            upper_quarter, -std::numeric_limits<long double>::infinity()
        );
        const long double above_upper_quarter = std::nextafter(
            upper_quarter, std::numeric_limits<long double>::infinity()
        );
        const std::vector<long double> boundary_values{
            -32.0L, -16.0L, std::nextafter(-16.0L, 0.0L),
            -4.0L, std::nextafter(-4.0L, 0.0L),
            below_six, 6.0L, above_six, 8.0L,
            below_upper_quarter, upper_quarter, above_upper_quarter,
            16.0L, 32.0L,
        };
        if(!coordinate_intervals_are_exact(
            boundary_values, 8.0L, 0.25L, 0.0L
        )) return false;
        if(!coordinate_intervals_are_exact(
            boundary_values, 8.0L, 1.5L, 0.0L
        )) return false;
        if(!coordinate_intervals_are_exact(
            boundary_values, -8.0L, 1.5L, 0.25L
        )) return false;
        if(!coordinate_intervals_are_exact(
            boundary_values, 0.0L, 0.75L, 0.0L
        )) return false;
        if(!coordinate_intervals_are_exact(
            boundary_values, -0.0L, 1.0L, 0.0L
        )) return false;

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

        constexpr std::array<long double, 9> relative_tolerances{
            1.0e-18L, 0.125L, 0.75L, 1.0L, 1.25L,
            1.5L, 1.9L, 2.0L, 3.0L
        };
        constexpr std::array<long double, 4> absolute_tolerances{
            0.0L, 1.0e-18L, 0.25L, 4.0L
        };
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t size = 1 + random() % 32;
            points.reserve(size);
            for(std::size_t index = 0; index < size; ++index){
                if(!points.empty() && random() % 9 == 0){
                    points.push_back(points[random() % points.size()]);
                    continue;
                }
                const auto coordinate = [&]{
                    if(random() % 3 == 0){
                        const long long value =
                            static_cast<long long>(random() % 257) - 128;
                        return static_cast<long double>(value) / 8.0L;
                    }
                    return random_finite_coordinate(random);
                };
                points.push_back({coordinate(), coordinate(), coordinate()});
            }
            const long double relative_tolerance =
                relative_tolerances[random() % relative_tolerances.size()];
            const long double absolute_tolerance =
                absolute_tolerances[random() % absolute_tolerances.size()];
            if(!same_sequence(
                deduplicate_points_3d_relative_range_tree(
                    points, relative_tolerance, absolute_tolerance
                ),
                brute_deduplicate(
                    points, relative_tolerance, absolute_tolerance
                )
            )) return false;

            std::vector<long double> values;
            values.reserve(points.size() + 8);
            for(const Point3& point: points) values.push_back(point.x);
            const long double fixed = points[random() % points.size()].y;
            values.push_back(fixed);
            values.push_back(std::nextafter(
                fixed, -std::numeric_limits<long double>::infinity()
            ));
            values.push_back(std::nextafter(
                fixed, std::numeric_limits<long double>::infinity()
            ));
            values.push_back(-std::abs(fixed));
            values.push_back(std::abs(fixed));
            if(!coordinate_intervals_are_exact(
                values, fixed,
                relative_tolerance, absolute_tolerance
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

        const long double overflow_threshold = maximum / 1.5L;
        const std::vector<long double> overflow_values{
            -maximum,
            std::nextafter(
                -overflow_threshold,
                -std::numeric_limits<long double>::infinity()
            ),
            -overflow_threshold,
            std::nextafter(-overflow_threshold, 0.0L),
            -1.0L, 0.0L, 1.0L,
            std::nextafter(overflow_threshold, 0.0L),
            overflow_threshold,
            std::nextafter(
                overflow_threshold,
                std::numeric_limits<long double>::infinity()
            ),
            maximum,
        };
        if(!coordinate_intervals_are_exact(
            overflow_values, 1.0L, 1.5L, 0.0L
        )) return false;
        if(!coordinate_intervals_are_exact(
            overflow_values, -1.0L, 1.5L, tiny
        )) return false;
        const long double large_fixed = maximum * 0.6L;
        const long double subtraction_boundary = maximum - large_fixed;
        const long double relative_boundary = large_fixed * 0.5L;
        const std::vector<long double> subtraction_overflow_values{
            -maximum,
            -maximum / 1.5L,
            std::nextafter(
                -subtraction_boundary,
                -std::numeric_limits<long double>::infinity()
            ),
            -subtraction_boundary,
            std::nextafter(-subtraction_boundary, 0.0L),
            std::nextafter(
                -relative_boundary,
                -std::numeric_limits<long double>::infinity()
            ),
            -relative_boundary,
            std::nextafter(-relative_boundary, 0.0L),
            0.0L, large_fixed, maximum,
        };
        if(!coordinate_intervals_are_exact(
            subtraction_overflow_values,
            large_fixed, 1.5L, 0.0L
        )) return false;
        if(!coordinate_intervals_are_exact(
            subtraction_overflow_values,
            -large_fixed, 1.5L, tiny
        )) return false;
        const std::vector<Point3> overflow_points{
            {1.0L, 1.0L, 1.0L},
            {maximum, 1.0L, 1.0L},
            {-maximum, 1.0L, 1.0L},
            {1.0L, maximum, 1.0L},
            {1.0L, 1.0L, maximum},
        };
        if(!same_sequence(
            deduplicate_points_3d_relative_range_tree(
                overflow_points, 1.5L, 0.0L
            ),
            brute_deduplicate(overflow_points, 1.5L, 0.0L)
        )) return false;

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

            std::vector<Point3> relative_many;
            constexpr std::size_t relative_unique_count = 8000;
            relative_many.reserve(relative_unique_count);
            for(std::size_t index = 0;
                index < relative_unique_count; ++index){
                const long double value =
                    static_cast<long double>(index + 1);
                relative_many.push_back({
                    value,
                    static_cast<long double>(
                        (index * 7919) % relative_unique_count + 1
                    ),
                    -value,
                });
            }
            const auto relative_unique =
                deduplicate_points_3d_relative_range_tree(
                    relative_many, 1.0e-20L, 0.0L
                );
            if(!same_sequence(relative_unique, relative_many)){
                return false;
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
        try{
            static_cast<void>(
                deduplicate_points_3d_relative_range_tree({}, 0.0L)
            );
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(
                deduplicate_points_3d_relative_range_tree({}, -1.0L)
            );
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(deduplicate_points_3d_relative_range_tree(
                {},
                std::numeric_limits<long double>::infinity()
            ));
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(
                deduplicate_points_3d_relative_range_tree(
                    {}, 0.5L, -1.0L
                )
            );
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(deduplicate_points_3d_relative_range_tree({{
                0, std::numeric_limits<long double>::quiet_NaN(), 0
            }}, 0.5L));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
